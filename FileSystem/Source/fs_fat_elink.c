/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2020 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_fat_elink.c
 * Purpose: FAT File System Entry Link Cache implementation
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "cmsis_compiler.h"

#include "fs_fat_elink.h"
#include "fs_evr.h"


/* Static function prototypes */
static ELINK   *elink_alloc        (uint32_t depth, ELINK_CACHE_CB *nc);
static void     elink_free         (ELINK *el, ELINK_CACHE_CB *nc);
static void     elink_con          (ELINK *nl, ELINK *el, uint32_t dir);
static void     elink_discon       (ELINK *el);


/**
  Connect entry link element at the end of the delete chain

  \param[in]  el                        entry link
  \param[in]  depth                     entry link depth
  \param[in]  nc                        name cache control block
*/
static void delete_list_add (ELINK *el, uint32_t depth, ELINK_CACHE_CB *nc) {
  ELINK *tl;

  /* This is the last link in chain */
  el->Del.Next = NULL;

  if (nc->Used[depth].Oldest == NULL) {
    /* Delete list is empty, set current as oldest */
    nc->Used[depth].Oldest = el;
    /* This is first link in chain */
    el->Del.Prev = NULL;
  }
  else {
    /* Connect link to the end of list */
    tl = nc->Used[depth].Latest;
    tl->Del.Next = el;
    el->Del.Prev = tl;
  }

  nc->Used[depth].Latest = el;
}


/**
  Disconnect given elink from the delete chain

  \param[in]  el                        entry link
  \param[in]  nc                        name cache control block
*/
static void delete_list_discon (ELINK *el, uint32_t depth, ELINK_CACHE_CB *nc) {

  if (el->Del.Prev == NULL) {
    /* Oldest link on the delete list */
    if (el->Del.Next) {
      el->Del.Next->Del.Prev = NULL;

      /* Update link to oldest used */
      nc->Used[depth].Oldest = el->Del.Next;
    }
    else {
      /* This is the one and only link in cache */
      nc->Used[depth].Latest = NULL;
      nc->Used[depth].Oldest = NULL;
    }
  }
  else if (el->Del.Next == NULL) {
    /* At the end of the delete list (latest one) */
    el->Del.Prev->Del.Next = NULL;

    /* Update link to latest used */
    nc->Used[depth].Latest = el->Del.Prev;
  }
  else {
    /* In the middle of the delete list */
    el->Del.Prev->Del.Next = el->Del.Next;
    el->Del.Next->Del.Prev = el->Del.Prev;
  }
}


/**
  Disconnect given elink from the delete chain and connect it at the end of chain.

  \param[in]  el                        entry link
  \param[in]  depth                     entry link depth
  \param[in]  nc                        name cache control block
*/
static void delete_list_update (ELINK *el, uint32_t depth, ELINK_CACHE_CB *nc) {

  /* Update link, if not at the end of delete list */
  if (el->Del.Next) {
    /* Disconnect link */
    delete_list_discon (el, depth, nc);

    /* Connect link as last used */
    delete_list_add (el, depth, nc);
  }
}


/**
  Get oldest entry link element from the delete link chain.

 This function is called when we are allocating new entry link and
 if Depth < MaxPathDepth.

  \param[in]  max_depth                 delete list max depth
  \param[in]  nc                        name cache control block
  \return     valid entry link pointer or
              NULL if delete list empty
*/
static ELINK *delete_list_get (uint32_t max_depth, ELINK_CACHE_CB *nc) {
  ELINK  *el;
  int32_t depth = (int32_t)max_depth - 1;

  do {
    /* Get oldest link */
    el = nc->Used[depth].Oldest;

    while (el) {
      if (!((depth == nc->Depth) && (el->Info.DirClus == nc->Dir))) {
        /* Disconnect entry link from entry chain */
        elink_discon (el);
        /* Disconnect entry link from the delete chain */
        delete_list_discon (el, (uint32_t)depth, nc);

        return (el);
      }
      el = el->Del.Next;
    }

    depth--;
  }
  while (depth >= 0);

  return (NULL);
}


/**
  Disconnect entry link element from the entry link chain

  \param[in]  el                        existing entry link
*/
static void elink_discon (ELINK *el) {

  if (el->Prev == NULL) {
    /* First link in chain */
    if (el->Next != NULL) {
      el->Next->Prev = NULL;
    }
  }
  else if (el->Next != NULL) {
    /* In the middle of the chain */
    el->Prev->Next = el->Next;
    el->Next->Prev = el->Prev;
  }
  else {
    /* At the end of the chain */
    el->Prev->Next = NULL;
  }
}


/**
  Connect entry link element into the entry link chain

  \param[in]  nl                        new entry link
  \param[in]  el                        existing entry link
  \param[in]  dir                       connection direction
*/
static void elink_con (ELINK *nl, ELINK *el, uint32_t dir) {
  ELINK *tl;

  if (dir == ELINK_DIR_PREV) {
    /* Connect link one before existing one */
    if (el->Prev == NULL) {
      /* First one in chain */
      nl->Prev = NULL;
      nl->Next = el;
      el->Prev = nl;
    }
    else {
      /* Insert link beetwen *(el-1) and *el */
      tl = el->Prev;

      tl->Next = nl;
      nl->Prev = tl;

      nl->Next = el;
      el->Prev = nl;
    }
  }
  else {
    if (dir == ELINK_DIR_NEXT) {
      /* Connect link one after existing one */
      if (el->Next == NULL) {
        /* Last one in chain */
        el->Next = nl;
        nl->Prev = el;
        nl->Next = NULL;
      }
      else {
        /* Insert link beetwen *el and *(e+1) */
        tl = el->Next;

        el->Next = nl;
        nl->Next = tl;
      
        nl->Prev = el;
        tl->Prev = nl;
      }
    }
  }
}


/**
  Add entry to the free link chain or create the first link in chain

  \param[in]  el                        entry link pointer
  \param[in]  nc                        name cache control block
*/
static void elink_free (ELINK *el, ELINK_CACHE_CB *nc) {
  el->Next = nc->Free;
  nc->Free = el;
}


/**
  Allocate entry link from the free list
  
  \param[in]  max_depth                 name cache structure depth
  \param[in]  nc                        name cache control block
  \return     pointer to free link or NULL if all links allocated
*/
static ELINK *elink_alloc (uint32_t max_depth, ELINK_CACHE_CB *nc) {
  ELINK *el;

  if (nc->Depth >= max_depth) {
    return (NULL);
  }

  /* Allocate free link */
  if (nc->Free) {
    /* Free links available */
    el = nc->Free;
    nc->Free = nc->Free->Next;
  }
  else {
    /* Free pool is empty, get link from the used list */
    el = delete_list_get (max_depth, nc);
  }

  if (el != NULL) {
    /* Clear ELINK structure */
    memset (el, 0, sizeof(ELINK));
  }

  return (el);
}


/**
  Flush elink cache for given drive

  \param[in]  max_depth                 maximum path depth
  \param[in]  p                         name cache instance
*/
static void elink_flush (uint32_t max_depth, ELINK_CACHE_CB *nc) {
  ELINK   *el;
  ELINK   *tl;
  uint32_t depth;

  depth = 0;

  do {
    /* Get oldest link */
    el = nc->Used[depth].Oldest;

    while (el) {
      tl = el->Del.Next;
      elink_free (el, nc);
      el = tl;
    }

    nc->Used[depth].Oldest = NULL;
    nc->Used[depth].Latest = NULL;

    depth++;
  }
  while (depth < max_depth);
}


/**
  Rewind elink cache and return first existing link from users directory

  \param[in]  dir                       directory cluster to rewind
  \param[in]  nc                        name cache control block
  \return     entry link or
              NULL if cache empty
*/
static ELINK *elink_rewind (uint32_t dir, ELINK_CACHE_CB *nc) {
  ELINK *el;

  el = nc->Used[nc->Depth].Oldest;

  if (el != NULL) {
    if (dir <= el->Info.DirClus) {
      /* Scan using prev link */
      while (el->Prev != NULL) {
        if (dir > el->Prev->Info.DirClus) {
          break;
        }
        el = el->Prev;
      }
    }
    else {
      /* Scan using next link */
      while (el->Next != NULL) {
        if (dir == el->Info.DirClus) {
          /* First entry in given dir */
          break;
        }
        el = el->Next;
      }
    }
  }
  return (el);
}


/* -------------------------- Exported functions -----------------------------*/


/**
  Get directory entry link from the chain

  \param[in]  nameH                     filename hash
  \param[in]  el                        entry link pointer address
  \param[in]  p                         name cache instance
  \return     execution status
                - 0 = link found, el contains valid data
                - 1 = name cache disabled or link not found (end of list)
*/
__WEAK uint32_t elink_scan (uint32_t nameH, ELINK **el, NCACHE *p) {
  ELINK_CACHE_CB *nc;
  ELINK          *tl;  /* Temp link */

  if (p == NULL) {
    return (1);
  }
  nc = (ELINK_CACHE_CB *)p->buf;

  tl = elink_rewind (nc->Dir, nc);

  while (tl != NULL) {
    if (tl->Info.DirClus == nc->Dir) {
      /* In the same directory */
      if (tl->Info.LongNameH == nameH || tl->Info.ShortNameH == nameH) {
        /* This is our entry */
        *el = tl;

        delete_list_update (tl, nc->Depth, nc);
        return (0);
      }
    }
    else {
      break;
    }
    tl = tl->Next;
  }
  return (1);
}


/**
  Disconnect entry link element from the entry link chain and put it into the
  free list.

  \param[in]  el                        existing entry link
  \param[in]  p                         name cache instance
  \return     execution status
                - 0 = ok
                - 1 = name cache disabled
*/
__WEAK uint32_t elink_delete (ELINK *el, NCACHE *p) {
  ELINK_CACHE_CB *nc;

  if (p == NULL) {
    return (1);
  }
  nc = (ELINK_CACHE_CB *)p->buf;

  if (el) {
    /* Disconnect link from the entry link chain */
    elink_discon (el);

    /* Disconnect link from the delete chain     */
    delete_list_discon (el, nc->Depth, nc);
  
    /* Put link back to the free list            */
    elink_free (el, nc);
  }
  return (0);
}


/**
  Insert entry link element into the link chain

  \param[in]  nl                        new entry link
  \param[in]  p                         name cache instance
  \return     execution status
                - 0 = ok
                - 1 = name cache disabled
*/
__WEAK uint32_t elink_insert (ELINK *nl, NCACHE *p) {
  ELINK_CACHE_CB *nc;
  ELINK          *el;
  uint32_t dir = 0;

  if (p == NULL) {
    return (1);
  }
  nc = (ELINK_CACHE_CB *)p->buf;

  /* Take oldest link from the used list */
  el = nc->Used[nc->Depth].Oldest;

  if (el == NULL) {
    /* First link in cache */
    delete_list_add (nl, nc->Depth, nc);
  }
  else {
    /* Some links already exist */
    if (nl->Info.DirClus < el->Info.DirClus) {
      dir = ELINK_DIR_PREV;

      /* Follow prev link */
      while (el->Prev != NULL) {
        if (nl->Info.DirClus > el->Prev->Info.DirClus) {
          break;
        }
        el = el->Prev;

        if (nl->Info.DirClus == el->Info.DirClus) {
          break;
        }
      }
    }
    else if (nl->Info.DirClus > el->Info.DirClus) {
      dir = ELINK_DIR_NEXT;

      /* Follow next link */
      while (el->Next != NULL) {
        if (nl->Info.DirClus < el->Next->Info.DirClus) {
          break;
        }
        el = el->Next;

        if (nl->Info.DirClus == el->Info.DirClus) {
          break;
        }
      }
    }

    if (nl->Info.DirClus == el->Info.DirClus) {
      /* Links from the same directory */
      if (nl->Info.EntryOffs < el->Info.EntryOffs) {
        dir = ELINK_DIR_PREV;

        /* Follow prev link */
        while (el->Prev != NULL) {
          if ((nl->Info.DirClus != el->Prev->Info.DirClus) ||
              (nl->Info.EntryOffs > el->Prev->Info.EntryOffs)) {
            break;
          }
          el = el->Prev;
        }
      }
      else if (nl->Info.EntryOffs > el->Info.EntryOffs) {
        dir = ELINK_DIR_NEXT;

        /* Follow next link */
        while (el->Next != NULL) {
          if ((nl->Info.DirClus != el->Next->Info.DirClus) ||
              (nl->Info.EntryOffs < el->Next->Info.EntryOffs)) {
            break;
          }
          el = el->Next;
        }
      }
    }

    if ((nl->Info.DirClus   == el->Info.DirClus) &&
        (nl->Info.EntryOffs == el->Info.EntryOffs)) {
      /* Already cached */
      delete_list_update (el, nc->Depth, nc);
      /* Release current entry link */
      elink_free (nl, nc);
    }
    else {
      delete_list_add (nl, nc->Depth, nc);
      elink_con (nl, el, dir);
    }
  }

  return (0);
}


/**
  Execute given command

  \param[in]  cmd                       command to execute
  \param[in]  p                         name cache instance
  \return     pointer to last used entry link
*/
__WEAK ELINK *elink_cmd (uint32_t cmd, NCACHE *p) {
  ELINK_CACHE_CB *nc;
  ELINK *el;

  if (p == NULL) {
    return (NULL);
  }
  nc = (ELINK_CACHE_CB *)p->buf;

  switch (cmd) {
    case ELINK_CMD_DIR_REWIND:
      /* Rewind to the beginning of users directory */
      el = elink_rewind (nc->Dir, nc);

      if (el != NULL) {
        delete_list_update (el, nc->Depth, nc);
      }
      break;

    case ELINK_CMD_POS_INC:
      /* Set next link as the latest used */
      el = nc->Used[nc->Depth].Latest->Next;

      if (el != NULL) {
        delete_list_update (el, nc->Depth, nc);
      }
      break;

    case ELINK_CMD_GET_LAST:
      /* Return last used entry link */
      break;

    case ELINK_CMD_ALLOC:
      /* Allocate name entry link */
      return (elink_alloc (p->max_path_depth, nc));

    case ELINK_CMD_CHDIR:
      /* Set current directory depth */
      nc->Depth_CD = nc->Depth;
      break;

    case ELINK_CMD_FLUSH:
      /* Flush name cache */
      elink_flush (p->max_path_depth, nc);
      break;
  }

  return (nc->Used[nc->Depth].Latest);
}


/**
  Set user directory location and control path depth

  \param[in]  dir                       directory cluster
  \param[in]  ctrl                      directory operation
  \param[in]  p                         name cache instance
  \return     execution status
                - 0 = success
                - 1 = failed
*/
__WEAK uint32_t elink_dir (uint32_t dir, uint32_t ctrl, NCACHE *p) {
  ELINK_CACHE_CB *nc;

  if (p == NULL) {
    /* FAT name cache is disabled */
    return (1);
  }
  nc = (ELINK_CACHE_CB *)p->buf;

  nc->Dir = dir;

  switch (ctrl) {
    case ELINK_DIR_ROOT:
      nc->Depth = 0;
      break;

    case ELINK_DIR_CD:
      nc->Depth = nc->Depth_CD;
      break;

    case ELINK_DIR_DEPTH_INC:
      nc->Depth++;
      break;

    case ELINK_DIR_DEPTH_DEC:
      nc->Depth--;
      break;
  }

  return (0);
}


/**
  Initialize entry indexing cache

  \param[in]  p                         name cache instance
  \return     execution status
                - 0 = OK
                - 1 = Indexing cache is not available
*/
__WEAK uint32_t elink_init (NCACHE *p) {
  ELINK_CACHE_CB *nc;
  ELINK   *el;
  uint32_t i, cnt, offs, poolSz;

  if (p == NULL) {
    /* FAT name cache is disabled */
    return (1);
  }
  /* Init elink control blocks */
  nc = (ELINK_CACHE_CB *)p->buf;

  nc->Dir      = 0;
  nc->Depth    = 0;
  nc->Depth_CD = 0;

  /* Init delete list pointers */
  nc->Used = (ELINK_USED *)&nc[1];

  for (i = 0; i < p->max_path_depth; i++) {
    nc->Used[i].Oldest = NULL;
    nc->Used[i].Latest = NULL;
  }

  /* Connect entry link pool to it's pointer */
  el = (ELINK *)&nc->Used[i];

  /* Determine how much memory pool is already used */
  offs = sizeof (ELINK_CACHE_CB) + (sizeof (ELINK_USED) * p->max_path_depth);

  /* Set memory pool size available for elink objects */
  poolSz = p->size - offs;

  /* Determine number of entry links we can hold in pool */
  cnt = poolSz / sizeof (ELINK);
  if (cnt) {
    i = cnt - 1;

    /* Init free link */
    nc->Free = &el[0];

    /* Mark last free link */
    el[i].Next = NULL;

    /* Connect all free entry links together */
    for (; i; i--) {
      el[i - 1].Next = &el[i];
    }
    /* Entry link cache initialized */
    return (0);
  }
  /* Entry link cache not available */
  return (1);
}
