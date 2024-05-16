/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::File System
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    fs_journal.c
 * Purpose: File System Journaling Implementation
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "cmsis_compiler.h"

#include "fs_journal.h"
#include "fs_common.h"


/*-----------------------------------------------------------------------------
 *        Set sector for the next journal transaction entry
 *  Parameters:
 *  fsj  - journal instance pointer
 *
 *  Returns: true if first sector of the next transaction was set
 *           false otherwise
 *----------------------------------------------------------------------------*/
static uint32_t SetTrSect (FSJOUR *fsj) {

  fsj->TrSect += JOUR_TR_SZ;

  if (fsj->FirstSect + fsj->JournSect - fsj->TrSect < JOUR_TR_SZ) {
    fsj->TrSect = fsj->FirstSect;
  }
  return (true);
}


/*-----------------------------------------------------------------------------
 *        Set new state
 *  Parameters:
 *  fsj  - journal instance pointer
 *
 *  Returns: void
 *----------------------------------------------------------------------------*/
static void StateInc (FSJOUR *fsj) {
  /* Journal state transition */
  switch (fsj->State) {
    case JST_IDLE: fsj->State = JST_COPY; break;
    case JST_COPY: fsj->State = JST_FOOT; break;
    case JST_FOOT: fsj->State = JST_CHNG; break;
    case JST_CHNG: fsj->State = JST_TEND; break;
    case JST_TEND: fsj->State = JST_COPY; break;
    
    default:       fsj->State = JST_IDLE; break;
  }
}


/*-----------------------------------------------------------------------------
 *        Set journal entry buffer
 *  Parameters:
 *  fsj  - journal instance pointer
 *  sect - number of the sector being journaled
 *  rsec - number of the redundant sector
 *
 *  Returns: true if buffer set
 *           false if journal is in the wrong state
*----------------------------------------------------------------------------*/
static uint32_t SetBuf (FSJOUR *fsj, uint32_t sect, uint32_t rsec) {
  /* Write file system journal header */
  uint32_t mark;

  /* Clear buffer */
  memset (fsj->buf, 0x00, __SECT_SZ);
  
  /* Set 1st journal signature */
  set_u32 (&fsj->buf[0], JOUR_SIGN);

  /* Set transaction number */
  set_u32 (&fsj->buf[4], fsj->TrId);

  switch (fsj->State) {
    case JST_FOOT:
      mark = JOUR_FOOT;
      break;

    case JST_TEND:
      mark = JOUR_TEND;
      break;

    default:
      return (false);
  }

  /* Set header marker */
  set_u32 (&fsj->buf[8], mark);

  /* Set original sector number */
  set_u32 (&fsj->buf[12], sect);

  /* Set redundant sector number */
  set_u32 (&fsj->buf[16], rsec);

  /* Set 2nd journal signature */
  set_u32 (&fsj->buf[508], JOUR_SIGN);

  return (true);
}


/*-----------------------------------------------------------------------------
 *        Check journal entry signature and fill in entry structure
 *  Parameters:
 *  fsj - journal instance pointer
 *  jen - journal entry structure pointer
 *
 *  Returns: true if sector signatures are valid
 *           false otherwise
 *----------------------------------------------------------------------------*/
static uint32_t CheckJSect (FSJOUR *fsj, JTR_EN *jen) {
  /* Check signatures */
  if (get_u32 (&fsj->buf[0]) == JOUR_SIGN && get_u32 (&fsj->buf[508]) == JOUR_SIGN) {
    jen->TrId = get_u32 (&fsj->buf[4]);
    jen->Mark = get_u32 (&fsj->buf[8]);
    jen->Sect = get_u32 (&fsj->buf[12]);
    jen->RSec = get_u32 (&fsj->buf[16]);
    return (true);
  }
  /* Invalid signatures */
  return (false);
}


/*-----------------------------------------------------------------------------
 *        Check if journal is in consistent state == all transactions are
 *        completed successfully. If journal is not consistent, previous state
 *        is restored if needed.
 *  Parameters:
 *  fsj - journal instance pointer
 *
 *  Returns: true if journal consistent or data restored 
 *           false if sector read/write error occured
 *----------------------------------------------------------------------------*/
static uint32_t CheckJourn (FSJOUR *fsj) {
  /* Check journal consistency */
  uint32_t co;
  uint32_t tr, sect;
  JTR_EN jen[2];

  uint32_t no_tr = fsj->JournSect / JOUR_TR_SZ;

  for (tr = 0; tr < no_tr; tr++) {
    /* Set journal transaction sector */
    sect = fsj->FirstSect + tr * JOUR_TR_SZ;

    /* Read foot of the transaction */
    if (fsj->drv->ReadSect (sect+1, fsj->buf, 1) == false) {
      /* Low level error */
      return (false);
    }    

    if (CheckJSect (fsj, &jen[0]) == false) {
      /* Footer signature invalid, skip this transaction */
    }
    else {
      /* Read end of the transaction */
      if (fsj->drv->ReadSect (sect+2, fsj->buf, 1) == false) {
        /* Low level error */
        return (false);
      }

      /* Determine number of the last transaction */
      if ((fsj->TrId < jen[0].TrId) || (fsj->TrId == 0xFFFFFFFF && jen[0].TrId == 0)) {
        fsj->TrId = jen[0].TrId;
        
        /* Set sector number for the new transaction */
        fsj->TrSect = sect + JOUR_TR_SZ;
        if (tr == (no_tr - 1)) {
          fsj->TrSect = fsj->FirstSect;
        }
      }
      
      co = false;

      if (CheckJSect (fsj, &jen[1]) == false) {
        /* Invalid signature, request restore */
        co = true;
      }

      if (jen[0].TrId != jen[1].TrId) {
        /* Transaction incomplete, request restore */
        co = true;
      }

      if (co == true) {
        /* Copy sector from journal to his place */
        if (fsj->drv->ReadSect (sect, fsj->buf, 1) == false) {
          /* Low level error */
          return (false);
        }
        
        if (fsj->drv->WriteSect (jen[0].Sect, fsj->buf, 1) == false) {
          /* Low level error */
          return (false);
        }

        if (jen[0].RSec && fsj->drv->WriteSect (jen[0].RSec, fsj->buf, 1) == false) {
          /* Low level error */
          return (false);
        }
        
        /* Restore done, clear journal entry */
        memset (fsj->buf, 0, __SECT_SZ);
        if (fsj->drv->WriteSect (sect+1, fsj->buf, 1) == false) {
          /* Low level error */
          return (false);
        }
        
        fsj->Status |= JSF_RESTORED;
        return (true);
      }
    }
  }
  return (true);
}


/*-----------------------------------------------------------------------------
 *        Set first sector number and number of sectors available to journal.
 *        Journal will be written in contiguous space starting from start_sect
 *        to start_sect + cnt.
 *  Parameters:
 *  fsj        - journal instance pointer
 *  start_sect - first sector reserved for the journal
 *  cnt        - total number of sectors reserved
 *
 *  Returns: true if enough sectors provided to write journal,
 *           false otherwise
 *----------------------------------------------------------------------------*/
__WEAK uint32_t fsj_set_space (FSJOUR *fsj, uint32_t start_sect, uint32_t cnt) {
  if (cnt < JOUR_TR_SZ) {
    /* Not enough or no space reserved to write journal */
    return (false);
  }

  /* File System provided space for journal */
  fsj->FirstSect = start_sect;
  fsj->JournSect = cnt;
  fsj->Status |= JSF_SPACERSVD;
  return (true);
}


/*-----------------------------------------------------------------------------
 *        Write journal. Original sector is written into the journal and
 *        restoration entry is added to the transaction structure. New sector
 *        is then written to its place and journal transaction is marked as
 *        completed.
 *  Parameters:
 *  fsj  - journal instance pointer
 *  sect - sector number to be journaled
 *  rsec - redundant sector number
 *  buf  - sector buffer pointer
 *
 *  Returns: true if transaction completed successfully
 *           false otherwise
 *----------------------------------------------------------------------------*/
__WEAK uint32_t fsj_write (FSJOUR *fsj, uint32_t sect, uint32_t rsec, uint8_t *buf) {
  /* Write FAT journal */
  uint32_t jour_sect;
  uint32_t wrs;
  uint8_t *wrb;

  if ((fsj->Status & JSF_INIT) == 0) {
    /* No init */
    return (false);
  }

  jour_sect = fsj->TrSect;
  do {
    /* Change state */
    StateInc (fsj);
    
    /* Set current write parameters */
    wrs = jour_sect;
    wrb = buf;

    switch (fsj->State) {

      case JST_COPY:
        /* Make a copy of the sector to be written */
        break;

      case JST_CHNG:
        /* Set write parameters to new sector */
        wrs = sect;

        /* Write redundant sector first */
        if (rsec && fsj->drv->WriteSect (rsec, wrb, 1) == false) {
          return (false);
        }
        /* Now overwrite original sector */
        break;
      
      case JST_FOOT:  /* Write footer sector  */
      case JST_TEND:  /* Write end sector     */
        wrb = fsj->buf;

        if (SetBuf (fsj, sect, rsec) == false) {
          /* Status/State update? */
          return (false);
        }
        break;
      
      default:
        return (false);
    }

    /* Write current sector */
    if (fsj->drv->WriteSect (wrs, wrb, 1) == false) {
      return (false);
    }

    /* Increase journal sector */
    if (fsj->State != JST_CHNG) {
      jour_sect++;
    }
  }
  while (fsj->State != JST_TEND);

  /* Set sector for the next transaction */
  if (SetTrSect (fsj) == false) {
    /* Sector fot the next journal transaction can not be obtained */
    return (false);
  }

  /* Increment transaction identifier */
  fsj->TrId++;

  return (true);
}


/*-----------------------------------------------------------------------------
 *        Init file system journal
 *        After successful init, journal is in JST_IDLE state.
 *  Parameters:
 *  fsj - journal instance pointer
 *  drv - memory device driver pointer
 *
 *  Returns: true if init successful
 *           false if data can not be read from or written to journal
 *----------------------------------------------------------------------------*/
__WEAK uint32_t fsj_init (FSJOUR *fsj, FAT_DRV *drv) {
  /* Init journaling system */
  fsj->drv = drv;

  if ((fsj->Status & JSF_SPACERSVD) == 0) {
    /* No space for journal provided */
    return (false);
  }

  fsj->TrId   = 0;                                /* Init transaction Id      */
  fsj->TrSect = fsj->FirstSect;                   /* Init itransaction sector */
  
  fsj->State   = JST_IDLE;
  fsj->Status |= JSF_INIT;

  /* Check journal consistency */
  if (CheckJourn (fsj) == false) {
    /* Journal inconsistent, there was write error */
    return (false);
  }

  /* Journal ready */
  return (true);
}
