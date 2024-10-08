# Secure Communication {#secure_communication}

In today's world, **secure communication** is crucial for many applications to protect data transmission between two
network nodes. The
[Transport Layer Security (TLS)](https://en.wikipedia.org/wiki/Transport_Layer_Security) protocol
is a standardized technology for establishing a secure, encrypted and authenticated link between two parties over an insecure
network. TLS is an industry standard and is used in millions of devices and websites.

The Network component offers two ways to secure communication in an application:
-# \ref use_mbed_tls
-# \ref use_secure_components

## Using Mbed TLS {#use_mbed_tls}

Although the Network component does not offer encryption and secure communication on its own, you can use Arm's
[mbed TLS](https://www.trustedfirmware.org/projects/mbed-tls/) software component to achieve this.

![Software stack for secure network communication](secure_com_sw_stack.png)

From the bottom up:

- The **Cortex-M-based microcontroller** provides the processor core, storage, memory and network interface.
- The **CMSIS-RTOS** compliant operating system provides standard services such as scheduling and thread-safety.
- The **Network Component** provides services, sockets (TCP/IP) and the interface for network communication.
- Building on top of the Network Component, **mbed TLS** provides an abstraction layer for secure communication.
- The **application code** uses Mbed TLS to abstract the secure communication from itself.

### Why Mbed TLS?

The Mbed TLS library is designed for ease-of-use. The library is documented and has examples so you can easily understand how
to use it. In the Network Component, Mbed TLS is used under the
[Apache 2.0 license](https://www.apache.org/licenses/LICENSE-2.0), enabling you to use it in both
open source and closed source projects. Mbed TLS is a fully featured and standards compliant SSL library offering server and
client functionality in one single package.

### Difference between SSL/TLS

The TLS protocol is the successor of the SSL protocol. Just like its predecessor, the TLS protocol provides communication
security for connections over possibly untrusted networks, like the Internet. The main difference between TLS and SSL is the
increased standardization of the workings of the protocol. SSL itself was designed and developed by Netscape. The newer TLS
standard is defined in a number of \ref mbed_rfcs "public RFCs" and is extended periodically to counter possible weaknesses
or add much needed functionality.

### Parts of an SSL/TLS Library

In order to perform the SSL or TLS protocol, a number of supporting functionality is required. The SSL/TLS library:

- needs to perform symmetric cryptographic operations, such as
  [AES](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard), to encrypt the data over the
  connection.
- uses asymmetric cryptographic operations, such as
  [RSA](https://en.wikipedia.org/wiki/RSA_%28cryptosystem%29), for identifying and authenticating
  the parties of the connection.
- uses message digest operations, such as the
  [SHA-256](https://en.wikipedia.org/wiki/SHA-2) hash algorithm, to protect the integrity of
  the information sent over the wire.
- needs to be able to parse, understand and use
  [X.509](https://en.wikipedia.org/wiki/X.509) certificates.
- has to perform network operations to send and receive the protocol packets.

All of this is hidden from most users and wrapped inside an SSL library, such as Mbed TLS, which developers can use to
implement SSL or TLS in their applications.

For more information on Mbed TLS and how it works, visit the
[high-level design](https://www.trustedfirmware.org/projects/mbed-tls/) overview page.

### Add the mbed TLS library to a project {#add_mbedtls}

The \ref examples section carries two examples for secure communication over the IP network: SSL Server and SSL Client.
To use the Mbed TLS library in your own projects, follow these steps:

-# Download the **ARM:mbedTLS** library from \urlout{SW-Pack} or use \urlout{uv4_ca_packinstaller}
-# Open or create a project using the **Network Component**.
-# Configure the Network Component as required by your application (Ethernet settings, TCP/IP communication, etc.).
-# In the **Manage Run-Time Environment** window expand **Security** and enable **mbed TLS**:
![Add mbed TLS](add_mbedTLS.png)
-# Configure Mbed TLS using the `mbedTLS_config.h` file.
-# Use the **mbed TLS** API to secure your communication.

\note You can use the Mbed TLS API in parallel to any of the \ref use_secure_components "secure services" that are
part of the Network Component.

## Using Secure Services {#use_secure_components}

The Network Component offers secure software components that are using \ref use_mbed_tls "Mbed TLS". The user of the
Network Component does not see the Mbed TLS API as it is hidden by the standard API of the secure component.

The following components are currently available in a secure variant:

- \ref https_server
- \ref smtps_client

To be able to communicate securely, you will need to generate appropriate certificates for the server. The section \ref cert_creation
explains how to achieve this for the secure components by using additional tools that are part of the Network Component.

### HTTPS Server {#https_server}

The web server and the compact web server have secure variants available. The example \ref HTTPS_Server_Example implements
the secure Web server.

> **Note**
>
> - The first connection might take a while (a few seconds up to 10s) and depends on the browser and how many sockets/sessions
>   it initially opens (differs in Edge, Chrome, Firefox, etc.). This delay is normal and due to the time required for
>   asymmetric cryptography calculations on the target. After the initial delay, the HTTPS server works almost as fast the HTTP
>   server.
> - Your browser will complain during the connection that the certificate has a problem or is not trusted. You will need to add
>   the certificate to your browser's trusted certificate storage manually.
> - Do not use the test certificate in productive environments as it is not secret. Before shipping your product, make sure
>   that you have added your \ref cert_creation "own certificates and keys".

#### Creating your own certificates and keys {#cert_creation}

The Network Component's HTTPS service adds the file `Net_Security.c` to the project. This file contains generic test keys/certificates
which enable the application to run out of the box. If you want to adapt the keys/certificates to your needs, you need utilities
`gen_key` and `cert_write` which are available as part of the [Arm MDK-toolbox](https://www.keil.arm.com/artifacts/#tools/arm/mdk-toolbox).

The certificates and keys are generated using bash shell commands:

- Generate random CA and Server Private Keys

  ```bash
  gen_key \
    type=rsa \
    rsa_keysize=2048 \
    format=pem \
    filename=ca.key
  ```

  ```bash
  gen_key \
    type=rsa \
    rsa_keysize=2048 \
    format=pem \
    filename=server.key
  ```

- Generate Self-signed CA Certificate

  ```bash
  cert_write \
    selfsign=1 \
    issuer_key=ca.key \
    issuer_name="CN=Test CA,O=MyOrganization,C=US" \
    not_before=20200101000000 \
    not_after=20301231235959 \
    is_ca=1 \
    output_file=ca.crt \
    san="DNS:my_host"
  ```

- Generate Server Certificate

  ```bash
  cert_write \
    subject_key=server.key \
    subject_name="CN=my_host,O=MyOrganization,C=US" \
    issuer_key=ca.key \
    issuer_crt=ca.crt \
    serial=2 \
    not_before=20200101000000 \
    not_after=20301231235959 \
    output_file=server.crt \
    san="DNS:my_host" 
  ```

The certificates and crypto keys are created and their contents must be copied to the file `Net_Security.c`:

- `ca.crt` into the array *NetSecurity_ServerCA[]*, with a leading " and a trailing \n" inserted for each line.
- `server.crt` into the array *NetSecurity_ServerCert[]*, with a leading " and a trailing \n" inserted for each line.
- `server.key` into the array *NetSecurity_ServerKey[]*, with a leading " and a trailing \n" inserted for each line.

**Code Example**

*ca.crt*

```text
-----BEGIN CERTIFICATE-----
MIIDUjCCAjqgAwIBAgIBATANBgkqhkiG9w0BAQsFADA4MRAwDgYDVQQDDAdUZXN0
IENBMRcwFQYDVQQKDA5NeU9yZ2FuaXphdGlvbjELMAkGA1UEBhMCVVMwHhcNMjAw
 ...
r3bk5HaVy8YUXog0H8mHTIp7x+i1Tq8QavMa/2bF3SOcNhvvEecG59zG9IfULqNv
fdCx0rfQujoMEHnwZjwEzBq5NdpPZi7heMYJKXlqHbHo+0itw9w=
-----END CERTIFICATE-----
```

*Net_Security.c*

```c
const uint8_t NetSecurity_ServerCA[] =
"-----BEGIN CERTIFICATE-----\n"
"MIIDUjCCAjqgAwIBAgIBATANBgkqhkiG9w0BAQsFADA4MRAwDgYDVQQDDAdUZXN0\n"
"IENBMRcwFQYDVQQKDA5NeU9yZ2FuaXphdGlvbjELMAkGA1UEBhMCVVMwHhcNMjAw\n"
 ...
"r3bk5HaVy8YUXog0H8mHTIp7x+i1Tq8QavMa/2bF3SOcNhvvEecG59zG9IfULqNv\n"
"fdCx0rfQujoMEHnwZjwEzBq5NdpPZi7heMYJKXlqHbHo+0itw9w=\n"
"-----END CERTIFICATE-----\n"
```

> **Note for Windows users**
>
> - You must add the test CA certificate `ca.crt` to the Trusted Root Certificate store on your computer
    to avoid the *Your connection is not private* error in the Chrome or Microsoft Edge browser.
> - You need administrator rights for adding a certificate.
> - To install a `ca.crt` certificate on your computer, double-click on it and select
    **Install Certificate...**, then select **Local Machine** for the store location.
> - Select **Trusted Root Certification Authorities** as the location for the certificate.

### SMTPS Client {#smtps_client}

The e-mail client is available in a secure variant. The \ref SMTPS_Client_Example example implements
the secure email client.

#### Adding server root CA certificate {#cert_adding}

The SMTPS service of the Network Component adds the `Net_Security.c` file to the project. This file contains an empty section
for the e-mail server root CA certificate, so you cannot initially build the application. You must provide a valid root CA certificate
for the server that you use to send e-mail. The certificate must be provided in PEM-encoding. Copy the contents of the certificate
into the NetSecurity_EmailServerCA structure in `Net_Security.c` and add a leading " and a trailing \n" to each line.

```c
// Email Server root CA certificate
const uint8_t NetSecurity_EmailServerCA[] =
"-----BEGIN CERTIFICATE-----\n"
"... base64 data ...\n"
"-----END CERTIFICATE-----\n";
```

Verifying the e-mail server is required by default to increase e-mail security. If you do not want to verify the server, you
can disable server verification by defining the **SMTPS_SERVER_VERIFY_NONE** in your project (C/C++ define).

> **Note**
> It is not necessary to create your own certificates or keys to send secure e-mail using SMTPS.
