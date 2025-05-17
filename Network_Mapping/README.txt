--------------------Raw Soketler ve Protokoller Hakkında --------------------
SOCK_RAW türündeki bir soket, IP başlığını ve alt protokolleri doğrudan kontrol etmenizi sağlar. Bu nedenle, protokol tipi doğru şekilde ayarlanmalıdır.

IPPROTO_* sabitleri, çeşitli protokoller için kullanılabilir. İşte en yaygın kullanılan bazı protokoller:

Yaygın Protokoller:

IPPROTO_ICMP:

Protokol: ICMP (Internet Control Message Protocol)
Kullanım: Ping, traceroute ve ICMP hata mesajları (örneğin, "destination unreachable") için kullanılır.
Örnek: ICMP echo request (ping) mesajları gönderirken bu protokolü kullanırsınız.

IPPROTO_TCP:
Protokol: TCP (Transmission Control Protocol)
Kullanım: TCP bağlantılarını, verileri akış halinde iletmek için kullanılır. Web tarama (HTTP), e-posta (SMTP), veri transferi (FTP) gibi birçok ağ uygulaması TCP üzerinden çalışır.
Örnek: TCP soket bağlantıları kurmak için kullanılır.

IPPROTO_UDP:
Protokol: UDP (User Datagram Protocol)
Kullanım: TCP'nin aksine, bağlantısız ve daha hızlı bir protokoldür. DNS sorguları, video akışı ve sesli iletişim gibi uygulamalar UDP kullanır.
Örnek: UDP ile paketler göndermek veya almak için kullanılır.

IPPROTO_GRE:

Protokol: GRE (Generic Routing Encapsulation)
Kullanım: GRE, tünelleme protokolüdür ve bir ağ üzerinden veri iletmek için başka bir protokolün kapsüllenmesini sağlar. Örneğin, VPN'ler için kullanılabilir.
Örnek: Tünelleme protokollerinde kullanılır.
IPPROTO_RAW:

Protokol: Raw IP (ham IP)
Kullanım: Bu protokol, IP başlığının tamamını ve veri kısmını doğrudan kontrol etmenizi sağlar. IP başlığını kendiniz oluşturabilir ve farklı protokolleri üst düzeyde ekleyebilirsiniz.
Örnek: Protokolü kendiniz kontrol etmek istiyorsanız kullanılır (genel amaçlı ham IP paketleri oluşturmak).
IPPROTO_IPV6:

Protokol: IPv6
Kullanım: IPv6 ağları için protokol, IPv6 başlıkları kullanarak ağ üzerinden veri iletmek için kullanılır.
Örnek: IPv6 ile veri iletmek için kullanılır.
IPPROTO_SCTP:

Protokol: SCTP (Stream Control Transmission Protocol)
Kullanım: TCP ve UDP'nin bazı özelliklerini birleştiren bir protokoldür. Video konferans ve sesli iletişim gibi uygulamalar için kullanılır.
Örnek: SCTP üzerinden veri iletmek için kullanılır.
IPPROTO_ESP:

Protokol: ESP (Encapsulating Security Payload)
Kullanım: IPsec ile güvenli tünelleme sağlar. Şifrelenmiş veri iletimi için kullanılır.
Örnek: IPsec tünellemelerinde kullanılır.
IPPROTO_AH:

Protokol: AH (Authentication Header)
Kullanım: IPsec güvenli bağlantılarında, veri doğruluğunu sağlamak için kimlik doğrulama başlıkları ekler.
Örnek: IPsec tünellemelerinde kimlik doğrulaması için kullanılır.

Bir IP Paketinin Yapısı:

-------------------------------------------------------------
|  Version  |  IHL  |  Type of Service  |   Total Length   |
-------------------------------------------------------------
|  Identification  |   Flags  |  Fragment Offset   |
-------------------------------------------------------------
|   Time to Live (TTL)  |  Protocol  |   Header Checksum   |
-------------------------------------------------------------
|        Source Address (32 bits)                       |
-------------------------------------------------------------
|        Destination Address (32 bits)                  |
-------------------------------------------------------------
|      Options (Optional)                               |
-------------------------------------------------------------
|                 Data (Payload)                        |
-------------------------------------------------------------


Bir IP paketi iki kısımdan oluşur: IP başlığı ve Veri bölümü(payload).

Bir ICMP Paketinin Yapısı:

------------------------------------------------------------
| IP Header                                                   |
------------------------------------------------------------
| Type (8 bits) | Code (8 bits) | Checksum (16 bits)          |
------------------------------------------------------------
| Rest of Header (32 bits)                                    |
------------------------------------------------------------
| ICMP Data (Payload)                                         |
------------------------------------------------------------



Bir TCP Paketinin Yapısı:

0                   1                   2                   3  
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-------------------------------+-------------------------------+
|        Source Port (16 bits)  |    Destination Port (16 bits) |
+-------------------------------+-------------------------------+
|                     Sequence Number (32 bits)                 |
+---------------------------------------------------------------+
|                 Acknowledgment Number (32 bits)               |
+-------+-------+-------+-------+-------------------------------+
| Data  | Res.  |  Flags|       Window Size (16 bits)           |
| Offset| (3bit)| (9bit)|                                       |
+-------------------------------+-------------------------------+
|      Checksum (16 bits)       |    Urgent Pointer (16 bits)    |
+-------------------------------+-------------------------------+
|         Options (variable, optional)         | Padding        |
+---------------------------------------------------------------+
|                      Data (Payload)                           |
+---------------------------------------------------------------+




Bir paketin yapısı:

------------------------------------------
|           Pseudo Header               |
------------------------------------------
|   Source IP  | Destination IP         |
|   Protocol   | TCP Length             |
------------------------------------------
       ↓
------------------------------------------
|             IP Header                 |
------------------------------------------
| Version | Header Length | Total Length |
| ID      | Flags         | TTL           |
| Source IP   | Destination IP        |
| Checksum   |
------------------------------------------
       ↓
------------------------------------------
|             TCP Header                |
------------------------------------------
| Source Port | Destination Port        |
| Sequence    | Acknowledgment         |
| Flags       | Window Size            |
| Checksum    | Urgent Pointer         |
------------------------------------------
       ↓
------------------------------------------
|                Data                   |
------------------------------------------



 Bir GRE Paketi Yapısı:
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 |C| Reserved0 | Ver | Protocol Type |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 | Checksum (optional) | Reserved1 (Optional) |
 +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-






















