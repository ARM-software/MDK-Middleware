/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2023 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    net_http_server_auth.c
 * Purpose: Web Server Authentication
 *----------------------------------------------------------------------------*/

#include <string.h>
#include "rl_net_lib.h"
#include "net_sys.h"
#include "net_common.h"
#include "net_mem.h"
#include "net_addr.h"
#include "net_md5.h"
#include "net_base64.h"
#include "net_http_server.h"
#include "net_dbg.h"

/* Local constants */

/* Local variables */

/* Exported functions */
static void    http_auth_init (void);
static void    http_auth_dummy (void);
static int32_t http_auth_basic_add  (void *session, char *buf);
static bool    http_auth_basic_parse  (void *session, char *buf);
static void    http_auth_digest_init (void);
static void    http_auth_digest_uninit (void);
static int32_t http_auth_digest_add (void *session, char *buf);
static bool    http_auth_digest_parse (void *session, char *buf);
static void    http_auth_digest_hash (const char *user,
                                      const char *passw, uint8_t *hash);

/* Local functions */
static uint8_t *get_rnd   (uint8_t *buf, uint32_t len);
static uint32_t get_nonce (char *buf);
static bool     verify_nonce (const NET_HTTP_TAG *auth);
static char    *get_next_token (char *buf);

/**
  \brief       Initialize HTTP authentication.
  \return      none.
*/
static void http_auth_init (void) {
  http->user      = httpc->User;
  http->passw     = httpc->Passw;
  http->user_len  = httpc->UserLen;
  http->passw_len = httpc->PasswLen;
  http->en_auth   = true;
}

/**
  \brief       Empty function.
  \return      none.
*/
static void http_auth_dummy (void) {
}

/**
  \brief       Add Basic authentication header.
  \param[in]   session  http session descriptor.
  \param[out]  buf      pointer to output buffer.
  \return      number of characters added.
*/
static int32_t http_auth_basic_add (void *session, char *buf) {
  int32_t len;

  (void)session;
  len  = net_strcpy (buf, "WWW-Authenticate: Basic realm=\"");
  len += net_strcpy (buf+len, httpc->Realm);
  len += net_strcpy (buf+len, "\"\r\n");
  return (len);
}

/**
  \brief       Parse Basic authentication scheme.
  \param[in]   session  http session descriptor.
  \param[in]   buf      pointer to basic auth field.
  \return      true if authenticated, false othervise.
*/
static bool http_auth_basic_parse (void *session, char *buf) {
  NET_HTTP_INFO *http_s = session;
  int32_t i;

  if (!net_strcasecmp (buf, "Basic ")) {
    return (false);
  }
  buf += 6;

  /* Decoded string written to the same buffer */
  i = (int32_t)net_base64_decode (&buf[0], &buf[0], 76);
  /* Add null-termination to credential string */
  buf[i] = 0;
  EvrNetHTTPs_ViewUserCredentials (buf, (uint32_t)i);

  if ((i = net_strchr (buf, ':')) >= 0) {
    /* Replace ':' separator to null-terminate username */
    buf[i] = 0;
    DEBUGF (HTTP," User [Password]: %s [%s]\n",&buf[0],&buf[i+1]);
    /* Check system admin account first */
    if (http->user && http->user_len && (strcmp (&buf[0], http->user) == 0)) {
      /* Username ok, check password */
      if (strcmp (&buf[i+1], http->passw) == 0) {
        /* Register admin-id */
        http_s->UserId = 0;
        return (true);
      }
    }
    /* Check optional external user accounts */
    else if ((i = netHTTPs_CheckAccount (&buf[0], &buf[i+1])) > 0) {
      /* Register user-id */
      http_s->UserId = i & 0xFF;
      return (true);
    }
  }
  /* Return error */
  return (false);
}

/* Exported HTTP Basic auth functions */
NET_HTTP_AUTH net_http_auth_basic_func = {
  http_auth_init,
  http_auth_dummy,
  http_auth_dummy,
  http_auth_basic_add,
  http_auth_basic_parse,
  NULL
};


/**
  \brief       Initialize Digest authentication.
  \return      none.
*/
static void http_auth_digest_init (void) {
  NET_HTTP_NONCE *p;
  uint32_t i;

  /* Init nonce cache table */
  for (i = httpc->NumNonce, p = &httpc->Nonce[0]; i > 0; p++, i--) {
    p->Ticks = 0;
    p->Count = 0;
  }
  http_auth_init ();
}

/**
  \brief       De-initialize Digest authentication.
  \return      none.
*/
static void http_auth_digest_uninit (void) {
  /* Clear nonce cache table */
  memset (httpc->Nonce, 0, sizeof(*httpc->Nonce) * httpc->NumNonce);
}

/**
  \brief       Add Digest authentication header.
  \param[in]   session  http session descriptor.
  \param[out]  buf      pointer to output buffer.
  \return      number of characters added.
*/
static int32_t http_auth_digest_add (void *session, char *buf) {
  NET_HTTP_INFO *http_s = session;
  int32_t len;

  len  = net_strcpy (buf, "WWW-Authenticate: Digest realm=\"");
  len += net_strcpy (buf+len, httpc->Realm);
  len += net_strcpy (buf+len, "\", qop=\"auth\", nonce=\"");
  len += get_nonce  (buf+len);
  len += net_strcpy (buf+len, "\", opaque=\"");
  len += net_bin2hex(buf+len, get_rnd ((uint8_t *)buf+len+8, 8), 8);
  len += net_strcpy (buf+len, "\"");
  if (http_s->Flags & HTTP_FLAG_STALE) {
    len += net_strcpy (buf+len, ", stale=TRUE");
  }
  /* Add CRLF termination */
  len += net_strcpy (buf+len, "\r\n");
  return (len);
}

/**
  \brief       Parse Digest authentication scheme.
  \param[in]   session  http session descriptor.
  \param[in]   buf      pointer to digest auth field.
  \return      true if authenticated, false othervise.
*/
static bool http_auth_digest_parse (void *session, char *buf) {
  NET_HTTP_INFO *http_s = session;
  NET_HTTP_TAG  *auth;
  NET_MD5_CTX   *ctx;
  const char *ha1;
  char ch,*fp;
  uint8_t *ha;
  int32_t i,n;
  bool retv;

  if (!net_strcasecmp (buf, "Digest ")) {
    return (false);
  }
  buf += 7;

  auth = __HTTP_TAG(net_mem_alloc (sizeof(NET_HTTP_TAG)));
  memset (auth, 0, sizeof(*auth));

  for (fp = buf, n = 0; fp; fp = get_next_token (fp)) {
    if (!auth->username.sp && net_strcmp (fp, "username=\"")) {
      auth->username.sp = fp += 10;
      i = net_strnchr (fp, '\"', NET_USERNAME_LEN);
      if (i > 0) {
        auth->username.len = i;
        fp += i+1;
      }
      n++;
      continue;
    }
    if (!auth->realm.sp && net_strcmp (fp, "realm=\"")) {
      auth->realm.sp = fp += 7;
      i = net_strnchr (fp, '\"', 32);
      if (i > 0) {
        auth->realm.len = i;
        fp += i+1;
      }
      n++;
      continue;
    }
    if (!auth->nonce.sp && net_strcmp (fp, "nonce=\"")) {
      auth->nonce.sp = fp += 7;
      i = net_strnchr (fp, '\"', 32);
      if (i > 0) {
        auth->nonce.len = i;
        fp += i+1;
      }
      n++;
      continue;
    }
    if (!auth->uri.sp && net_strcmp (fp, "uri=\"")) {
      auth->uri.sp = fp += 5;
      /* GET uri + query can be very long */
      i = net_strnchr (fp, '\"', 400);
      if (i > 0) {
        auth->uri.len = i;
        fp += i+1;
      }
      n++;
      continue;
    }
    if (!auth->qop.sp && net_strcmp (fp, "qop=")) {
      if (fp[4] == '"') {
        /* Quoted "qop-value" */
        fp++; ch = '"';
      }
      else    ch = ',';
      auth->qop.sp = fp += 4;
      i = net_strnchr (fp, ch, 8);
      if (i > 0) {
        auth->qop.len = i;
        fp += i;
      }
      n++;
      continue;
    }
    if (!auth->nc.sp && net_strcmp (fp, "nc=")) {
      auth->nc.sp = fp += 3;
      i = net_strnchr (fp, ',', 12);
      if (i > 0) {
        auth->nc.len = i;
        fp += i;
      }
      n++;
      continue;
    }
    if (!auth->cnonce.sp && net_strcmp (fp, "cnonce=\"")) {
      auth->cnonce.sp = fp += 8;
      i = net_strnchr (fp, '\"', 64);
      if (i > 0) {
        auth->cnonce.len = i;
        fp += i+1;
      }
      n++;
      continue;
    }
    if (!auth->response.sp && net_strcmp (fp, "response=\"")) {
      auth->response.sp = fp += 10;
      i = net_strnchr (fp, '\"', 36);
      if (i > 0) {
        auth->response.len = i;
        fp += i+1;
      }
      n++;
      continue;
    }
    if (!auth->opaque.sp && net_strcmp (fp, "opaque=\"")) {
      auth->opaque.sp = fp += 8;
      i = net_strnchr (fp, '\"', 32);
      if (i > 0) {
        auth->opaque.len = i;
        fp += i+1;
      }
      n++;
      continue;
    }
  }

  retv = false;

  /* Check if all tokens are found */
  if (n != (sizeof(*auth) / sizeof(NET_TAG_INFO))) {
    goto exit;
  }

  /* Add null-termination to username */
  (CONST_CAST(char *)auth->username.sp)[auth->username.len] = 0;
  DEBUGF (HTTP," Username-Digest: %s\n",auth->username.sp);
  EvrNetHTTPs_ViewUserCredentials (auth->username.sp, (uint32_t)auth->username.len);

  /* Check system admin account first */
  if (http->user && http->user_len && strcmp (auth->username.sp, http->user) == 0) {
    /* Register admin-id */
    http_s->UserId = 0;
  }
  /* Check optional external user accounts */
  else if ((i = netHTTPs_CheckAccount (auth->username.sp, "")) > 0) {
    /* Register user-id */
    http_s->UserId = i & 0xFF;
  }
  else {
    /* Not valid user */
    goto exit;
  }

  ha  = (uint8_t *)net_mem_alloc (64 + sizeof (NET_MD5_CTX));
  ctx = __MD5_CTX(ha + 64);

  /* Check if secret word is MD5 hash */
  if (http_s->UserId > 0) {
    /* Retrive secret word for user account */
    ha[0] = 0;
    netHTTPs_GetUserSecret (http_s->UserId, (char *)&ha[0], 32+1);
    ha[32] = 0;
    ha[33] = strlen((char *)&ha[0]) & 0xFF;
    if (ha[33] == 32) {
      /* Secret word is HA1 hash */
      goto calc_ha2;
    }
  }
  else if (http->passw_len == 32) {
    /* Admin secret word is HA1 hash */
    goto calc_ha2;
  }

  /* Calculate HA1 = MD5(username : realm : password) */
  net_md5_init (ctx);
  net_md5_update (ctx, auth->username.sp, auth->username.len);
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, httpc->Realm, httpc->RealmLen);
  net_md5_update (ctx, ":", 1);
  if (http_s->UserId == 0) {
    net_md5_update (ctx, http->passw, http->passw_len);
  }
  else {
    net_md5_update (ctx, (char *)&ha[0], ha[33]);
  }
  net_md5_final (ctx, &ha[16]);

  /* Convert MD5 hash to hex string HA1 */
  net_bin2hex ((char *)&ha[0], &ha[16], 16);

  /* Calculate HA2 = MD5(method : uri) */
calc_ha2:
  net_md5_init (ctx);
  switch (http_s->Method) {
    case HTTP_METHOD_GET:
      net_md5_update (ctx, "GET:", 4);
      break;
    case HTTP_METHOD_POST:
      net_md5_update (ctx, "POST:", 5);
      break;
    case HTTP_METHOD_HEAD:
      net_md5_update (ctx, "HEAD:", 5);
      break;
    default:
      break;
  }
  net_md5_update (ctx, auth->uri.sp, auth->uri.len);
  net_md5_final (ctx, &ha[48]);

  /* Convert MD5 hash to hex string HA2 */
  net_bin2hex ((char *)&ha[32], &ha[48], 16);

  /* Calculate MD5(HA1 : nonce : nc : cnonce : qop : HA2) */
  net_md5_init (ctx);
  ha1 = (char *)&ha[0];
  if ((http_s->UserId == 0) && (http->passw_len == 32)) {
    /* Admin password is HA1 */
    ha1 = http->passw;
  }
  net_md5_update (ctx, ha1, 32);
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, auth->nonce.sp, auth->nonce.len);
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, auth->nc.sp, auth->nc.len);
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, auth->cnonce.sp, auth->cnonce.len);
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, auth->qop.sp, auth->qop.len);
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, (char *)&ha[32], 32);
  net_md5_final (ctx, &ha[16]);

  /* Convert MD5 hash to hex string */
  net_bin2hex ((char *)&ha[0], &ha[16], 16);

  /* Check digest-response */
  if (memcmp (&ha[0], auth->response.sp, 32) == 0) {
    /* Ok, perform nonce verification */
    if (verify_nonce (auth)) {
      retv = true;
    }
    else {
      /* The client may wish to simply retry the request with a */
      /* new encrypted response, without reprompting the user   */
      /* for a new username and password.  (RFC2617 page 10)    */
      http_s->Flags |= HTTP_FLAG_STALE;
    }
  }
  net_mem_free (__FRAME(ha));

exit:
  net_mem_free (__FRAME(auth));
  return (retv);
}

/**
  \brief       Calculate MD5 hash value HA1.
  \param[in]   user   pointer to username.
  \param[in]   passw  pointer to password.
  \param[out]  hash   buffer to store MD5 hash value to.
  \return      none.
*/
static void http_auth_digest_hash (const char *user,
                                   const char *passw, uint8_t *hash) {
  NET_MD5_CTX *ctx;

  /* Calculate HA1 = MD5(username : realm : password) */
  ctx = __MD5_CTX(net_mem_alloc (sizeof (NET_MD5_CTX)));
  net_md5_init (ctx);
  net_md5_update (ctx, user, (int32_t)strlen(user));
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, httpc->Realm, httpc->RealmLen);
  net_md5_update (ctx, ":", 1);
  net_md5_update (ctx, passw, (int32_t)strlen(passw));
  net_md5_final (ctx, hash);
  net_mem_free (__FRAME(ctx));
}

/**
  \brief       Generate random data.
  \param[out]  buf    buffer for the random data.
  \param[in]   len    number of random bytes required.
  \return      pointer to buffer.
*/
static uint8_t *get_rnd (uint8_t *buf, uint32_t len) {
  uint32_t i,rnd;

  for (i = rnd = 0; i < len; rnd >>= 8, i++) {
    if (rnd == 0) {
      rnd = net_rand32 ();
    }
    buf[i] = rnd & 0xFF;
  }
  return (buf);
}

/**
  \brief       Generate nonce random number.
  \param[out]  buf  buffer for the nonce random number.
  \return      length of nonce value.
*/
static uint32_t get_nonce (char *buf) {
  NET_HTTP_NONCE *p,*ca;
  uint32_t i,ticks = sys->Ticks;

  /* Check all nonce cache entries */
  for (i = httpc->NumNonce, p = ca = &httpc->Nonce[0]; i > 0; p++, i--) {
    if (p->Ticks == 0) {
      /* Entry not used */
      ca = p;
      break;
    }
    if ((ticks - p->Ticks) > (ticks - ca->Ticks)) {
      /* Entry is older */
      ca = p;
    }
  }
  /* Entry selected */
  ca->Count = 0;
  ca->Ticks = sys->Ticks;
  get_rnd ((uint8_t *)ca->Val, 18);
  net_base64_encode (buf, ca->Val, 18);
  memcpy (ca->Val, buf, 24);
  return (24);
}

/**
  \brief       Verify nonce random number.
  \param[out]  auth  parsed authentication tags.
  \return      - true:  nonce valid,
               - false: expired or not existing nonce.
*/
static bool verify_nonce (const NET_HTTP_TAG *auth) {
  NET_HTTP_NONCE *p;
  uint32_t i,nc,ticks = sys->Ticks;

  if (auth->nonce.len != sizeof(p->Val)) {
    return (false);
  }
  /* Check all nonce cache entries */
  for (i = httpc->NumNonce, p = &httpc->Nonce[0]; i > 0; p++, i--) {
    if ((ticks - p->Ticks) >= httpc->NonceTout) {
      /* Entry has expired */
      continue;
    }
    if (memcmp (p->Val, auth->nonce.sp, (uint32_t)auth->nonce.len) == 0) {
      /* Check nonce count to prevent replay attacks */
      nc = net_xtoul (auth->nc.sp, NULL);
      if (nc >= p->Count) {
        /* Update count to next expected value */
        p->Count = (nc + 1) & 0xFFFF;
        return (true);
      }
    }
  }
  return (false);
}

/**
  \brief       Parse authentication line and search for next token.
  \param[in]   buf  buffer containing http header.
  \return      pointer to next token or NULL.
  \note        Auth tokens are separated with ',' characters. The line may
               be broken into several lines with soft line breaks. (CRLF + ' ')
*/
static char *get_next_token (char *buf) {
  char *bp,*end = buf + 128;

  for (bp = buf; bp < end; bp++) {
    if (bp[0] == ',' || bp[0] == '\r') {
      break;
    }
  }
  if (bp == end) {
    /* Safety limitation */
    return (NULL);
  }
  if (bp[0] == ',') bp++;
  if (bp[0] == '\r' && bp[1] == '\n') {
    /* End of line CRLF */
    if (bp[2] != ' ') {
      /* No more tokens */
      return (NULL);
    }
    bp += 3;
  }
  /* Skip leading spaces */
  while (bp[0] == ' ') bp++;
  return (bp);
}

/* Exported HTTP Digest auth functions */
NET_HTTP_AUTH net_http_auth_digest_func = {
  http_auth_digest_init,
  http_auth_digest_uninit,
  http_auth_dummy,
  http_auth_digest_add,
  http_auth_digest_parse,
  http_auth_digest_hash
};

/* Library default function(s) */
__WEAK uint8_t netHTTPs_CheckAccount (const char *username, const char *password) {
  (void)username; (void)password; return (0); }
__WEAK void netHTTPs_GetUserSecret (uint8_t user_id, char *buf, uint32_t buf_len) {
  (void)user_id; (void)buf; (void)buf_len; }
