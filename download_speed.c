/**
 * Client: Angela Mendez
 * Bidder: Tan M. (nhattanmai91@gmail.com)
 * Date: 23-Nov-2014 (05:00 am)
 * Status: on-going 
 * Version: 1
 * Previous version: no
 * TODO: Download speed
 */

/**
 * USAGE:
 * Compile: gcc main.c -o main
 * Run: ./main
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#define PORT 80
#define RCVBUFSIZE 4096

unsigned long time_ms();
unsigned long ResolveName( char name[] );
void SplitString(char *first, char *second,  char *stringtosplit);
void FileName( char *filename, char *url, int length );
void DieWithError( char *message );


char *link_pic = "http://speedtest.fpt.vn/speedtest/random4000x4000.jpg";
unsigned long byte_downloaded=0;
unsigned long start_ms=0, end_ms=0, download_time=0;
float speed = 0;

int main( int argc, char *argv[] ) {
   int sock, i, length, bytes, size, csize;
   struct sockaddr_in server;
 char *fileurl[4], buffer[RCVBUFSIZE], *command="";
// FILE *fp;

 size = strlen( link_pic ) + 1;

 for( i = 0 ; i < 4 ; i++ ) 
  fileurl[i] = malloc(size);

 SplitString ( fileurl[2], fileurl[1], link_pic );
 FileName ( fileurl[3], link_pic, strlen(link_pic) );

 csize = strlen( fileurl[1] );
 command = malloc ( csize + 50 );

 /* build the request we are going to send */
 strcpy(command, "GET ");
 strcat(command, fileurl[1]);
 strcat(command, " HTTP/1.1\r\nHost: ");
 strcat(command, fileurl[2] );
 strcat(command, "\r\n\r\n");

 /* create reliable stream socket */
   if( ( sock = socket ( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
      DieWithError( "socket() failed" );

   /* Construct the server address structure */
   memset ( &server, 0, sizeof( server ) );
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = ResolveName ( fileurl[2] );
   server.sin_port = htons ( PORT );

   /* establish a connection to the server */
   if( connect ( sock, ( struct sockaddr * )&server, sizeof( server ) ) < 0 )
     DieWithError( "connect() failed!" );

 length = strlen( command );
 //fp = fopen("testfile", "w" );
// if(fp==NULL)
//   DieWithError("Opening file failed");

 if( send( sock, command, length, 0 ) != length )
   DieWithError( "send() sent a different number of bytes than expected" );

 start_ms = time_ms();

 while((bytes = recv (sock, buffer, sizeof(buffer), 0)) > 0){
   //fwrite (buffer, bytes, 1, fp);
   byte_downloaded += bytes;
   if(byte_downloaded>=200000)
     break;
 }

 end_ms=time_ms();
 download_time = end_ms-start_ms;
 speed=(float)byte_downloaded/(float)download_time;
 printf("Time (ms): %ld\n", download_time); 
 printf("Downloaded (Byte): %ld\n", byte_downloaded);
 printf("Speed (KB/s): %f\n", speed); 
 
 close(sock);
// fclose(fp);

 for( i = 0 ; i < 4 ; i++ )
   free( fileurl[i] );
 free(command);
   exit(0);
}

unsigned long time_ms()
{
   struct timeval tv; 
   if(gettimeofday(&tv, NULL) != 0) return 0;
   return (unsigned long)((tv.tv_sec * 1000ul) + (tv.tv_usec / 1000ul));
}

unsigned long ResolveName( char name[] ) {
 struct hostent *host;
 if( ( host = gethostbyname(name) ) == NULL ) {

  fprintf(stderr, "gethostbyname() failed\n");
  exit(1);
 }
 /* return the binary, network-byte-ordered address */
 return *( ( unsigned long * ) host->h_addr );
}

void SplitString(char *first, char *second,  char *stringtosplit) {
 char *q, *r, *s;

 q = stringtosplit;
 r = second;
 s = first;

 int k=0,j=0,i=0;
 if( q[5]=='/' && q[6]=='/')
  i = 7;

 for(;q[i]!='/';i++,j++)
  s[j]=q[i];

 for(; q[i]!='\0';i++,k++)
  r[k]=q[i];

 r[k] = '\0';
 s[j] = '\0';
}

void FileName( char *filename, char *url, int length ) {
 int i, pos = 0;
 for(i = 0 ; i < length ; i++ ) {
    if( url[i] == '/' )
      pos = i;
 }
 pos++;

 for( i = 0; pos < length ; pos++, i++ ) {
    filename[i] = url[pos];
 }

 filename[i] = '\0';
}

void DieWithError( char *message ) {
 perror( message );
 exit(1);
}
