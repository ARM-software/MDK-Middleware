/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2024 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    Net_Security.c
 * Purpose: Network Security Credentials
 * Rev.:    V7.2.0
 *----------------------------------------------------------------------------*/

#include "rl_net.h"


#if defined(RTE_Network_Web_Server_RO_TLS) || \
    defined(RTE_Network_Web_Server_FS_TLS)

// Web Server (HTTPS)

// Server CA certificate(s)
extern const uint8_t NetSecurity_ServerCA[];
       const uint8_t NetSecurity_ServerCA[] =
//NS_SERVER_CA_START
// Issuer:      :C=US, O=MyOrganization, CN=Test CA
// Subject:     :C=US, O=MyOrganization, CN=Test CA
// Valid From:  :1.01.2020 01:00:00
// Valid To:    :1.01.2031 00:59:59
// Serial No:   :01
// Signature    :sha256RSA
// Key Size:    :2048
// Key Exchange :RSA-PKCS1-KeyEx
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDUjCCAjqgAwIBAgIBATANBgkqhkiG9w0BAQsFADA4MRAwDgYDVQQDDAdUZXN0\r\n"
"IENBMRcwFQYDVQQKDA5NeU9yZ2FuaXphdGlvbjELMAkGA1UEBhMCVVMwHhcNMjAw\r\n"
"MTAxMDAwMDAwWhcNMzAxMjMxMjM1OTU5WjA4MRAwDgYDVQQDDAdUZXN0IENBMRcw\r\n"
"FQYDVQQKDA5NeU9yZ2FuaXphdGlvbjELMAkGA1UEBhMCVVMwggEiMA0GCSqGSIb3\r\n"
"DQEBAQUAA4IBDwAwggEKAoIBAQDJ3ix/lcTQTXRxjoDuc0pDY2/Kx1qTA6R7INQT\r\n"
"FcZ7T2Y2+pHhMBPh/yDYzvqjEQHfD/nHbaw7oigo0opjeQV1XIROgGI4JLT6G5Pi\r\n"
"1HVIb/5SxVI5/3AS6gpKr+RHXBNqlgfhcSAMR9jqaP5fojy0cNEOU6okBmHzn1Dp\r\n"
"9mVIBR8/LMl02oHSjS1myDIFJutmEu2+mL7fap6AverGEj/4uz4NPMAQpWhPO+rh\r\n"
"cEYhz0tgmC3so22Dsvb9WSns0+YVNJhT2e3IHALfCroDeRQ23QQ91Y64b07HOCO+\r\n"
"n7KkWjo6zo2mV6XPdXjxKPzoKypUut5ak724vvg/QioT3ZBPAgMBAAGjZzBlMA8G\r\n"
"A1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFCjnB0BpoPKCptJNs0vCPzDZ5Dg0MB8G\r\n"
"A1UdIwQYMBaAFCjnB0BpoPKCptJNs0vCPzDZ5Dg0MBIGA1UdEQQLMAmCB215X2hv\r\n"
"c3QwDQYJKoZIhvcNAQELBQADggEBALQ5/sitrGD/yKS1NDetii5pbj185JodKzHh\r\n"
"K9JJXDGYXUVVt6sl/ZNsQ3MM+Y6xmfjF3wpTj+3NOV4vF4blCrzDUvUPuMl4jIXC\r\n"
"I9VceSxJWPaMp/RCvgOlYyxtBIDQGGdqdj563DENswsw/Kj8LfVjmn7qVcRj8sVT\r\n"
"n1rg5k05UrJ8YDSK6bQW4dGrHTW6zcLCgrvHW2KrcxmsDpWjcN586og9jPfDvwOE\r\n"
"+vPtXrZNtBKwNrvRbjGJPVro1Alflnr065K039uJpvSF/sEEEMI4e8nyYPvOjmHu\r\n"
"pv1+rKQ13wYvuGf9460TsLsC+cw+m1ViJDl+a4CW14AfwAXm0Gs=\r\n"
"-----END CERTIFICATE-----\r\n"
;
//NS_SERVER_CA_END

// Server certificate
extern const uint8_t NetSecurity_ServerCert[];
       const uint8_t NetSecurity_ServerCert[] =
//NS_SERVER_CERT_START
// Issuer:      :C=US, O=MyOrganization, CN=Test CA
// Subject:     :C=US, O=MyOrganization, CN=my_host
// Valid From:  :1.01.2020 01:00:00
// Valid To:    :1.01.2031 00:59:59
// Serial No:   :02
// Signature    :sha256RSA
// Key Size:    :2048
// Key Exchange :RSA-PKCS1-KeyEx
"-----BEGIN CERTIFICATE-----\r\n"
"MIIDTDCCAjSgAwIBAgIBAjANBgkqhkiG9w0BAQsFADA4MRAwDgYDVQQDDAdUZXN0\r\n"
"IENBMRcwFQYDVQQKDA5NeU9yZ2FuaXphdGlvbjELMAkGA1UEBhMCVVMwHhcNMjAw\r\n"
"MTAxMDAwMDAwWhcNMzAxMjMxMjM1OTU5WjA4MRAwDgYDVQQDDAdteV9ob3N0MRcw\r\n"
"FQYDVQQKDA5NeU9yZ2FuaXphdGlvbjELMAkGA1UEBhMCVVMwggEiMA0GCSqGSIb3\r\n"
"DQEBAQUAA4IBDwAwggEKAoIBAQDLSSC7xVIin0z13QAzJpJXMFCTEunT0n3r8onh\r\n"
"7OfAGNTWI6nbrPKGdmlvGdhm7wr40X1bvKyVl2UVuHB8FcUuZcIdoskZKczCMgo4\r\n"
"03Lze9Vf2mH/e8stwSL24s5kNomy7JHuhqXnaConi3PWMYi3jnMhI3DEypU9GDYb\r\n"
"bzihrgv6mwjjtMWH0Z2FWeQmPmXIupEozwpeen5TfnEuE7AhOLy7juViUPdod6HE\r\n"
"DLkf0sJ0ubZKCPJRxboCZmVL2H7AJJf58kBhj2DT3OrsaEmGSBaPGNwEpsaqi0Tc\r\n"
"+y8l5aDr0h5qmKpI7Ae0x1pYKKD+EzEl1j1VadNN56L2DWLFAgMBAAGjYTBfMAkG\r\n"
"A1UdEwQCMAAwHQYDVR0OBBYEFBVBouVm0owt0mTjLvnla4Tm/2svMB8GA1UdIwQY\r\n"
"MBaAFCjnB0BpoPKCptJNs0vCPzDZ5Dg0MBIGA1UdEQQLMAmCB215X2hvc3QwDQYJ\r\n"
"KoZIhvcNAQELBQADggEBAE87JMPsEHxIX4VuSfB8QTQHWQ003ypRQm2uA5mXyIda\r\n"
"blIDKmgO/dRnZCwXyRBHg2bZr7C6n3GEh87xJNSTx5BYBsDmfVE2iIsuF4ACLt/W\r\n"
"U0N3/mXEUxFi3ImISEVSecsB7zdJ3jkGGK4HtKiFNFnDCmBiLIH8G2RUcm1WCvsh\r\n"
"1r4L/lw7GfaZkd34rvYaMdq6DMw9e1YJ8AeJUYet9CyFEuJINORd5iKT3GIdeoap\r\n"
"bxJPNa2vea4vb79/88b7C8NbertLPLYs2M8+uyGajcwMmjGSrv8x2qTm5dMLOWLO\r\n"
"Vy9JQ8kD9vFPN2DfNovTIukNbRGw0nRC0qlpWVnPXBo=\r\n"
"-----END CERTIFICATE-----\r\n"
;
//NS_SERVER_CERT_END

// Server private key
extern const uint8_t NetSecurity_ServerKey[];
       const uint8_t NetSecurity_ServerKey[] =
//NS_SERVER_KEY_START
"-----BEGIN RSA PRIVATE KEY-----\r\n"
"MIIEpAIBAAKCAQEAy0kgu8VSIp9M9d0AMyaSVzBQkxLp09J96/KJ4eznwBjU1iOp\r\n"
"26zyhnZpbxnYZu8K+NF9W7yslZdlFbhwfBXFLmXCHaLJGSnMwjIKONNy83vVX9ph\r\n"
"/3vLLcEi9uLOZDaJsuyR7oal52gqJ4tz1jGIt45zISNwxMqVPRg2G284oa4L+psI\r\n"
"47TFh9GdhVnkJj5lyLqRKM8KXnp+U35xLhOwITi8u47lYlD3aHehxAy5H9LCdLm2\r\n"
"SgjyUcW6AmZlS9h+wCSX+fJAYY9g09zq7GhJhkgWjxjcBKbGqotE3PsvJeWg69Ie\r\n"
"apiqSOwHtMdaWCig/hMxJdY9VWnTTeei9g1ixQIDAQABAoIBAERDJynGheE4awh4\r\n"
"kNOcVTHP+I6aoEhP+5bal/B0tBGuhPHvgd9RgRMj+h2jqSZPiPiqqBTBE5ccAoa7\r\n"
"q5IPYWpGI1v+SNcZUTe4w/P3FTer0qfgVl4LPNv9afdLjBDcu3qItCxLrLQ7J0Sm\r\n"
"MFyxv34b6I8tLnX2rQyZ77lgrLylcwlDer1wFnbPEqnT47AHd3Vik8jxFGMaUnp7\r\n"
"2obrwqW5Bi4OS/rcfEzvS22y0s7BDgjgO8pIOUXDegRS9fLt9rEalZDFzCBeZ8bm\r\n"
"yIN8eMSjnxp1Usg0NHBImypTZourb9d3M5Ia8u5vjbs6ZdxPH/XUWvtVzgg67fBc\r\n"
"C+V0mzUCgYEA5zdGbYbXgK9sFupKwzoB3uhKOLLahvKVt+TalVdYL6yatyoPobt2\r\n"
"DctwaLr1TuuP1F9BIEu8LfnG3wn62DSdhFQsrBt7UQ4kCbOHbbyTjAlkBGQFJp8J\r\n"
"YMKPKs+WLATXPRiHU5NuGBbsf0MF9B3zU4bSPX7nHCHuEBD0DywYPycCgYEA4RNt\r\n"
"UIrcpFPQGUh47u0zPQLTF97EWa5smlu4tFBKWCGQWsDfFQO8jKbiwwWxGYZgxS8q\r\n"
"Gq5fJtlpbLrfOSChbnTvG48LT4aqp7yxlHjoVph9nQy476WtEkjxRzYLTXj/1N9p\r\n"
"sy0UP2C+hB6ai5JWYfyDihDmBF0DMyQUvTIBgjMCgYEAjLnH08HOSDR8xTW0az5T\r\n"
"BSi8g+XXu2rKVTKlF2Ah/GJBiFnBSd6e66Z2tEeWJoKY99lB+jYeQp1TvUyFsDHV\r\n"
"7USOK/JOC/dfV8g9FT1BUGomxcVanTcOriW8nYHbFM+dcUIMm4GP8HmDV7GjXgEc\r\n"
"qCnWrDcg0q6UBpNK+DEOUOECgYEAjJvEWOLHQGmTwZfC5RCOR/kxwuHKLHSJKE1Q\r\n"
"sGTQbQpa0jdXphs1eXDwjQFLSDTo04bJeO2iR/TepayAs4GtGAI5NjBA0qfAiRWa\r\n"
"orR+AUs2fhxqlFweKSQkh2Y93WJQAL6pm+P3mDhqDMPJrJP5HzHPQzuXVvZ8f0B1\r\n"
"LFTHGH0CgYAXFoowvsSO7WfxOAzNOczJk0h/ElZHhzSMG6ucOualSqFvpZrh1PNO\r\n"
"JkPwjqgJ+TPE1WsswYIhsIt0pNW9vEm97sw22LlHBsN6obnX4Mxx+nV83/CpMiVl\r\n"
"6A9XBJ+eis/Thz3Wt3Bkq1KnaABoWdUgxmh7QMQmsqpWrzWxNkhOTQ==\r\n"
"-----END RSA PRIVATE KEY-----\r\n"
;
//NS_SERVER_KEY_END

extern const uint32_t NetSecurity_ServerCA_Len;
       const uint32_t NetSecurity_ServerCA_Len   = sizeof(NetSecurity_ServerCA);
extern const uint32_t NetSecurity_ServerCert_Len;
       const uint32_t NetSecurity_ServerCert_Len = sizeof(NetSecurity_ServerCert);
extern const uint32_t NetSecurity_ServerKey_Len;
       const uint32_t NetSecurity_ServerKey_Len  = sizeof(NetSecurity_ServerKey);

#endif


#if defined(RTE_Network_SMTP_Client_TLS) && \
   !defined(SMTPS_SERVER_VERIFY_NONE)

// Email Client (SMTPS)

// Email Server root CA certificate
extern const uint8_t NetSecurity_EmailServerCA[];
       const uint8_t NetSecurity_EmailServerCA[] =
"-----BEGIN CERTIFICATE-----\r\n"
#error "Email Server CA certificate is missing!"
/*
  This certificate is used to verify the identity of the email server. It must
  be PEM-encoded. Provide a certificate or disable server verification with
  the following SMTPS_SERVER_VERIFY_NONE macro definition.
*/
/*
"...base64 data...\n"
*/
"-----END CERTIFICATE-----\r\n";

extern const uint32_t NetSecurity_EmailServerCA_Len;
       const uint32_t NetSecurity_EmailServerCA_Len = sizeof(NetSecurity_EmailServerCA);

#endif
