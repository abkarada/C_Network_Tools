


---DNS Header(12 byte)---

+---------------------+
|       ID (16 bit)  |  --> Sorgu Kimliği
+---------------------+
|QR | OPCODE |AA|TC|RD|RA| Z | RCODE (16 bit)|
+---------------------+
|    QDCOUNT (16 bit) |  --> Sorgu Sayısı
+---------------------+
|    ANCOUNT (16 bit) |  --> Yanıt Sayısı
+---------------------+
|    NSCOUNT (16 bit) |  --> Yetkili Ad Sunucu Sayısı
+---------------------+
|    ARCOUNT (16 bit) |  --> Ek Kayıt Sayısı
+---------------------+

QR(1 bit):0 Sorgu(Query), 1 = Yanıt(Response)
OPCODE(4 bit): 0 = standart sorgu / 1 = ters sorgu(IP->Alan adı)/2 = Sunucu durumu sorgusu 
AA (1 bit): Yetkili yanıt verildi mi? (1 = Evet)
TC (1 bit): Mesaj çok büyükse kesildi mi? (1 = Evet)
RD (1 bit): Rekürsif sorgu yap (1 = Evet, recursive query istendi)
RA (1 bit): Sunucu rekürsif sorgu destekliyor mu? (1 = Evet)
Z (3 bit): Gelecekte kullanım için ayrılmıştır.
QDCOUNT (16 bit):Sorgu içinde kaç tane alan adı sorgulandığını gösterir.
ANCOUNT (16 bit):Sunucudan dönen yanıt sayısıdır.
NSCOUNT(16 bit):Yetkili ad sunucularının sayısı
ARCOUNT(16 bit):Extra kaynak kayıtlarının sayısıdır.


+------------------------------+
|          HEADER (12B)        |  -> ID, Flags, QDCOUNT, ANCOUNT, NSCOUNT, ARCOUNT
+------------------------------+
|       QUERY SECTION          |  -> QNAME (Alan Adı)
|  QTYPE (2B) | QCLASS (2B)    |
+------------------------------+
|      ANSWER SECTION          |  -> Yanıtlar buraya gelir
+------------------------------+
|      AUTHORITY SECTION       |  -> Yetkili sunucu kayıtları
+------------------------------+
|      ADDITIONAL SECTION      |  -> Ek bilgiler
+------------------------------+


Bir DNS sorgusunda, ne tür bir kayıt almak istediğimizi belirten bir sorgu türü (QTYPE) vardır.

A	1	Alan adını IPv4 adresine çözümler
AAAA	28	Alan adını IPv6 adresine çözümler
CNAME	5	Takma adları gösterir
MX	15	Mail Exchange (E-posta sunucusu) kaydı
NS	2	Yetkili ad sunucusu
PTR	12	IP adresini alan adına çevirir (Ters DNS)
SOA	6	Yetkili sunucu bilgisi
TXT	16	Alan adı ile ilgili ek bilgiler içerir




Sorgu sınıfı (QCLASS), DNS sorgusunun hangi protokol grubuna ait olduğunu belirler.

Sınıf	Değer	Açıklama
IN	1	İnternet (En yaygın)
CH	3	Chaosnet (Tarihi bir sistem)
HS	4	Hesiod (MIT tarafından kullanılan bir sistem)

















