struct sockaddr{
sa_family sa_family;//Adres ailesi-->AF_INET,AF_INET6 or AF_UNIX
char sa_data[14];
}

sockaddr yapısı genellikle struct sockaddr_in veya struct sockaddr_in6 gibi yapılara
dönüştürülerek kullanılır.

struct sockaddr_in{//IPv4 için özel yapı
sa_ family sin_family;
in_port sin_port;
struct in_addr sin_addr;//IP adresi
char sin_zero[8];
}

struct in_addr{
uint32_t s_addr;/*32-bit IP adresi(network byte order)
		genelde inet_addr() veya inet_aton() ile atanır;
}		big-endian formatında saklanır.*/
----Kütüphaneye Özel Fonksiyonlar--------

socket fonksiyonu-->sys/types ve sys/socket kütüphanelerini kullanır.
int socket(int domain, int type, int protocol);//domain->hangi adres ailesinin
					//kullanılacağını belirler.
				//type-->Bağlantı türünü tanımlar TCP?UDP
			//protocol-->kullanılaca protocolü belirler
Başarıda->Dosya tanıtıcı(fd) döndürür(pozitif bir tamsayı)
hatada -> -1 döndürür.

----------------------------------------
bind fonksiyonu--->sys/types ve sys/socket kütüphanelerini kullanır 
//bind(), oluşturulan bir soketi belirli bir 
//IP adresi ve port numarasına bağlamak için kullanılır.

int bind(int socketfd, cosnt struct sockaddr -addr, socklen_t addlen);
//socketfd-->socket() fonksiyonu tarafından döndürülen soket 
//dosya tanıtıcısı (file descriptor).
	//addr-->Bağlacak IP adresi ve port bilgisini içeren yapı(sockaddr_in)
--addrlen-->addr yapısının boyutu-->sizeof(struct sockaddr_in);
Başarıda -> 0
Hatada -> -1 döner.
Olası bind hataları 
Address already in use	Aynı IP/port ile başka bir soket zaten bağlı.
Bu hatada lsof -i :port numarasına bakıp sonrasında kill PID ile bağlı olan socketi
kullanımdan kaldırabilirsiniz.
-------------------------------------
Permission denied
1024’ten küçük portları root izni olmadan bağlamak yasaktır.
---------------------------------
Invalid argument	
sockaddr veya addrlen yanlış.
------------------------------------------
listen fonksiyonu bağlantı kuyruğunu açar ve gelen bağlantıları dinlemeye başlar.
int listen(int sockfd, int backlog);
Başarıda-->0
Hatada--> -1 döner.
++++++++++++++++++++++++++++++++++++++++++
accept fonksiyonu beklemeye alınmış bir istemci bağlantısnı kabul eder ve yeni bir
soket oluşturur
int accept(int socketfd, (struct sockaddr *)addr, socket_len *addrlen);
Başarıda ->Yeni bir soket tanıtıcısı(fd) döndürür.
Hatada -> -1
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
pthread(Posix Thread)
multithreading programlamayı sağllayan bir kütüphanedir.Çok çekirdekli işlemcilerde
paralel işlem yaparak programların daha verimli çalışmasını sağlar.

Temel bir thread oluşturmak 
#include <pthread.h>->kütüphane tanımlanır.

Fonskiyon tanımlanır.

pthread_t threadID;// Thread tanımlaması.
		|
phtread_create(&threadID, NULL, fonksiyon, NULL);//Thread oluşturulması.
/*int thread_create(pthread_t *thread, const pthread_attr_t, 
void *(*start_routine)(void *), void *arg)*/
pthread_join(threadID, NULL);// Thread bitene kadar bekleme--Join

+++Birden fazla thread kullanımı++++
Fonksiyon tanımlanır.
void* threadFunction(void* arg) {
    int id = *(int*)arg;
    printf("Thread %d çalışıyor.\n", id);
    return NULL;
}

pthread_t threads[3];//3 farklı thread 
int threadIDs[3] = {1, 2, 3};//thread e gönderilen parametreler.

for(int i = 0; i< 3; i++){
	pthread_create(&threads[i], NULL, threadFunction, &threadIDs[i]);/*Gönder-
ilecek parametre kısmına threadID lerini girdik.*/
}
// Thread'lerin bitmesini bekle
 for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }


+++++Veri Yarışı Önleme(Mutex kullanımı)++++++++
Çoklu thread kullanılırken aynı değişkene aynı anda erişilirse veri yarışları(race
condition) oluşabilir.
Bunu önlemek için kilitleme mekanizması(pthread_mutex_t) kullanılır.

pthread_mutex lock;//Mutex tanımlanır.
int counter = 0; // Paylaşılan değişken

void* func(void* arg) türünden fonksiyon tanımlanır
fonsiyonun içinde paylaşılan değişkenle ilgili bir işlem bulunur
multi threading te aynı değişkene birden fazla ve aynı anda thread
parçacığı işlemeye çalışırsa yanlış sonuçlar oluşabilir.
bu yüzden 

pthread_mutex_lock(&lock);/* ile değişken kilide alınır kilitliyken aynı anda
sadece bir thread e açıktır diğer threadlerin değişkeni değiştirmesi
engellenir.*/
counter++;/*Counterla ilgili işlem
pthread_mutex_unlock(&lock);//Kilidi bırakır artık diğer iş parçacağı değişkene
müdahale edebilir.*/

main kısmında->

pthread_t threads[5];
pthread_mutex_init(&lock, NULL);//Mutex başlat

for(int i = 0; i < 5; i++){
	pthread_create(&threads[i], NULL, fonksiyon, NULL(argüman göndermiyoruz);

}
   for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&lock); // Mutex'i temizle



pthread_detach() Kullanımı
Bazı thread'leri ana programın beklemesini istemiyorsak, pthread_detach() 
fonksiyonunu kullanabiliriz.
 

pthread_t threadID;
pthread_create(&threadID, NULL, threadFunction, NULL);
pthread_detach(threadID);

bağımsız olarak çalışır program bitince otomatik olarak temizlenir.

pthread_exit() Kullanımı
Eğer bir thread'in bitmesini istiyorsak, pthread_exit() fonksiyonunu kullanabiliriz.

void* myThread(void* arg) {
    printf("Thread çalışıyor.\n");
    pthread_exit(NULL);
}



++++++++Ağ Programlarken Neden Threadlere İhtiyaç Duyarız++++++++++++
accept() gibi fonksiyonlar blocking(engelleyici) çalışır yani bir istemci 
bağlanana kadar program burada bekliyor.Ayrıca, accept() sonrasında sunucu
yalnıc tek bir istemciyi işleyebiliyor ve diğer istemcileri blok ediyor.

Bu sorunu çözmek için çoklu iş parçacığı (multithreading) kullanabiliriz. 
Her bağlantıyı kabul eden bir ana thread olacak
ve her istemciyi işleyen ayrı bir thread oluşturulacak.

1.Ana thread sürekli accept() çağrısı yaparsa sürekli bağlanan soketFD lerini
döndürür.
2.Her yeni client için ayrı bir thread oluşturulur.
3.Thread fonksiyonu istemciden veriyi alır ve işleme devam eder
4.Client bağlantısı kesildiğinde threaed kapanır.


****pthread_create() thread oluşturur ve bir fonksiyona(void* parametre alan)
çalıştırmasını sağlar.

Çalıştırılacak fonksiyonun adresi (void* dönüş tipinde olmalı)
fonksiyona gönderilecek parametreler(argümanlar) void* türünde olmalı

int *clientSocketFD = malloc(sizeof(int));/*clientsocketFD için boş bir bellek
alanı tahsis et nede?Çünkü accept() fonksiyonu bize int türünden bir fd döndürecek
bu dönen fd yi buraya yerleştirmek için bunu yaptık*/
 *clientSocketFD = accept(socketfd, (struct sockaddr *)&clientAddress, &clientAd
dressSize);

/* *clientSocketFD ne demek? malloc la oluşturduğumuz bellek adresindeki değerleri
gösteren bir işaretçi kullandık peki işaretçi(pointer) nedir?
İşaretçi basitçe bir adresste tutulan değerleri gösterir.
malloc la oluşturduğumuz belleğin adresindeki değerleri güncelliyoruz
eğer sadece clientSocketFD yi güncelleseydik bu sadece adresi değiştirir
ve hataya yol açardı 
Mesela; adress = 0x803475983 ve  adreste tutulan değer = 3 bizim amacımız bellek 
adresinde tutulan değeri yani (0x803475983) -> 3 ü değiştirmek 
(gördün mü -> ile işaret ettik)adresi yani 0x803475983 i değiştirmek değil.
*/

if(*clientSocketFD <0 ){
	perror("Accept failed");
	free(clientSocketFD);
	contiune;
}
pthread_t threadID;
pthread_create(&threadID, NULL, handleClient, clientSocketFD);
pthread_detach(threadID);  // Thread'in otomatik olarak temizlenmesini sağla

void* handleClient(void *arg){
	int clientSocketFD = *(int *)arg;//void* parametreyi int* olarak çeviriyor.
	free(arg); // Dinamik olarak ayrılan bellek temizleniyor
	
	char buffer[1024];
	
	while(true){
	ssize_t amountReceived = recv(clientSocketFD, buffer, sizeof(buffer)-1,0);
	/*size_t ile ssize_t arasındaki fark size_t sadece pozitif ve 0 tamsayılar
	tutar ama bizim dönüş değerimiz negatifte olabilir(hata durumunda) bu 
	yüzden negatif değerleride alan ssize_t tanımlayıcısını kullandık ki
	alınan verinin büyüklüğü 0 ın altına düştüğünde müdahale edip
	döngüyü kıralım*/
	if(amountReceived > 0){
	buffer[amountReceived] = 0;/*Bunu neden yapıyoruz?Çünkü bize gelecek olan
	veri bir char dizisi ve C dilinde char dizilerinin sonunda '\0' (NULL)
	bulunur bu da  okumanın oraya kadar olduğunu belirler
	buffer dizisi oluşturulduğunda null karakteri 1024.ü indekse yerleştirildi
	biz ise gelen verileri okumak istiyoruz bunun için gelen mesajları bir
	char dizisi olarak alıp sonuna \0 ekliyoruz ki okuyabilelim.*/
	printf("Client sent: %s\n", buffer);
	}
	if(amountReceived <= 0)//Hata kontrolü
	{
		break;//Döngüyü kır
	}

	
}

}
  close(clientSocketFD);
    printf("Client disconnected.\n");
    return NULL;

}

Kodu derlemek için: gcc -o Server SocketServer.c -pthread 
		    gcc -o Client SocketClient.c -pthread
		    
Çalıştırmak için: ./Server
		  ./Client
		  
		  
