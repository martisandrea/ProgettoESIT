#include <pgmspace.h>
const char ssid[] = "HUAWEI P30 lite";//"HUAWEI P30 lite"//"PosteMobile-84940176"//"IoT-UNICA";
const char password[] = "ce93f56dabc1";//"ce93f56dabc1"//"C2s4hds647"//"I@T_unic@2019";
#define THINGNAME "esit_obj1"
int8_t TIME_ZONE = +1; //ITALY +1 UTC
#define USE_SUMMER_TIME_DST
const char MQTT_HOST[] = "a38y4cqpqfo89o-ats.iot.eu-north-1.amazonaws.com";
// Copy contents from AWS CA certificate here ▼
static const char cacert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char client_cert[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUQRwUucbdT5ZORQUKkoljj8njrlIwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTIxMzA5MzIw
MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAL9hv/rqmKVbCwZsChMW
BOln6y0RcWvD1Vl4YQTl/fixaYzzVkvAquKmCRdw3LHMD3/d6mKfEFuQkeIwCaO4
gMcz8DFQ4ijHgK8HQQruNbYN5b7cgPPUZ3CVUtOxMp7HAVgkI0Faqy2DnvQ+sib2
oiqDYA5Fgm0IWwErdAmO5DyGpD4B3OXLH02DMDA2TjcfERmRBAoMAT5Yi5haXJRJ
/G+rr+0jkG0UDVOZRsP6uD7AzRx5BCjYXJwU0q5tGAi4QEPxAMzkMwtQ0CiA+sT8
l+tgj7aXXikU4MLiTPmX2SBAnmDnY16bJ995etKYZKwFy/f86zqFd4sAeVLCVLIC
IKsCAwEAAaNgMF4wHwYDVR0jBBgwFoAUR9vAOwgh8MVZMo9jlOoY0zT0UK4wHQYD
VR0OBBYEFN+Sno3KL7n+0UDf8QfOWIQ1PTZKMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBWmIAx2emcSD9DQzn+2A28Zmn2
KVl3gpHm3kVdTza/cFy3xqsdbIZ1htMpNmGOaJRb6thqT+PhAyVQD0A0M4681djy
hPHLxwc/smFs91DojcfFs4JXeRLRpW+39tuzJzTJyUbAdOLZjnjfZtUrtqGW3oIj
ixRV3zvyiBphgQ4H9RK8sXV1UNZo7R9q4xniqgiS1rP1GjQ3wwZuTv7UvyEqoyAo
PCUzY3qHGQrxYjb/JGO4fqaEEDMz1EQOq4bydhdQYk/t4xnlp9xEYWQdbYjcdjUy
3izoWQaJ2C9/lDuWAfn/lqMlFcTGyxVt83TxYOr2Eg4FvUUZsbjeNYMmLEBW
-----END CERTIFICATE-----

)KEY";
// Copy contents from XXXXXXXX-private.pem.key here ▼
static const char privkey[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAv2G/+uqYpVsLBmwKExYE6WfrLRFxa8PVWXhhBOX9+LFpjPNW
S8Cq4qYJF3DcscwPf93qYp8QW5CR4jAJo7iAxzPwMVDiKMeArwdBCu41tg3lvtyA
89RncJVS07EynscBWCQjQVqrLYOe9D6yJvaiKoNgDkWCbQhbASt0CY7kPIakPgHc
5csfTYMwMDZONx8RGZEECgwBPliLmFpclEn8b6uv7SOQbRQNU5lGw/q4PsDNHHkE
KNhcnBTSrm0YCLhAQ/EAzOQzC1DQKID6xPyX62CPtpdeKRTgwuJM+ZfZIECeYOdj
Xpsn33l60phkrAXL9/zrOoV3iwB5UsJUsgIgqwIDAQABAoIBAQCXqr66+emouYL+
PuapA302/+znVXh+P783XWf9usrPqMGOO1W7SN/kKFLyKbYsjRwHaHWUXJxZBDQo
fYtWHSSpOWbznjbsvFYl32jTYb5dHZxh6lujYEPte9kc/MOyAuH02Qa7h0A7HKD+
zSTgLfqMGHsqNDnNcJ62SZp27K/Bqk9lZf5B0FufYurbv1F1yHWptwAZnW4PIfy9
WeQeTvccBrNFWca0hNoY7hkBq1P/NsUiyJ1jvR3N7wIQcqqTU6u7LxB6uaZC7RqR
9A9BsOtosRlP05iwQrGw2litejix48++7z04atMSTTYbvUYxbC/qIc8Lk+2Y269y
lUb7guqZAoGBAODVbDUBvo14Dm9tOZoeC6pstQwcZD62gnryI802ZCtmeMWNvBEQ
SbTLlPwFM4mi15biTk1HR0qI8doHPRSVxOUBU1gS83mJ1bae4X5V6hA479IGXLwF
b/lED+yiQ9Jj3UxmUPrlPJw7WvNXoixkgUSGUqmCdhxgg+HyNRwa6zOfAoGBANnp
PFuia7P9CyRYSsTNT9BxIDKAixRQx0sWHn/AJeWk8qn1qUK2yEC8JTpqm6P3jZG4
8DNm5RuzcKcyw8bAtIQItdzErNeC0Ji5+pGlEbnkNh/suUSXbji+PmEINSejAXw/
J9WAXQjnAo4Khz9g8d0TJaFDAo83v9Y547kbN9d1AoGBAN1YP9dPiFGuSEfauELK
IaH+s57gtLAo7fwsNqLpUWSI1Y1oJCaIaofbhSk31a1C7AxXb88UxVLTz4ZeafvC
Fr19uJeU0K5uzD+l20mNO4oNA805BkL092/nWeYVOb//IPb+I+tdk3Yb2UaIl9gs
ejcEJo1LKwx02GGxGig5OCTxAoGADT18q1ytCpyfZq/uqxKGfPXCrQ7eZHecBFXt
uHlmVBO3otgIVsNgW36TDH+ZwVazRV1lSto/pXU7rQ0KlDL1ROh6lFVm4xlRr1Xt
gZf0AcV5/GqXvzEsQ0pb4+1pvWTMHE6CDxjjjjzrDNJa8WDkkTgumyPFLgqHH28Z
BRBATlkCgYEAtFBb6sz82IeCsWl7DzRJq14+64ByCdfwI0cVSnxeTsQmUAkV5vsI
86fw3QpxraRj2fPqr0BYjHxJUcBCjHGvZSHZQzbc56QrFECy+OlMzDpJTJrL2TZW
AEeCJcJf3NVnHpRsfEiaaPSU1UfeGh/oyBFPwRpOTv5yfhjG1AYcrYA=
-----END RSA PRIVATE KEY-----
)KEY";