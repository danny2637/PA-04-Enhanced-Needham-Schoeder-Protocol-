/*----------------------------------------------------------------------------
PA-04:  Part One Intro to Enhanced Needham-Schroeder Key-Exchange with TWO-way Authentication

FILE:   amal.c

Written By:
     1- Samuel George
	 2- Danny Ramos   (or risk losing points )
Submitted on:
     11/10/2026
----------------------------------------------------------------------------*/

#include <linux/random.h>
#include <time.h>
#include <stdlib.h>

#include "../myCrypto.h"

// Generate random nonces for Amal
void  getNonce4Amal( int which , Nonce_t  value )
{
	// Normally we generate random nonces using
	// RAND_bytes( (unsigned char *) value , NONCELEN  );
	// However, for grading purpose, we will use fixed values

	switch ( which )
	{
		case 1:		// the first nonce
			value[0] = 0x11223344 ;
			break ;

		case 2:		// the second nonce
			value[0] = 0xaabbccdd ;
			break ;

		default:	// Invalid agrument. Must be either 1 or 2
			fprintf( stderr , "\n\nAmal trying to create an Invalid nonce\n exiting\n\n");
			exit(-1);
	}
}

//*************************************
// The Main Loop
//*************************************
int main ( int argc , char * argv[] )
{
    int      fd_A2K , fd_K2A , fd_A2B , fd_B2A  ;
    FILE    *log ;

    char *developerName = "Code by George & Ramos" ;

    fprintf( stdout , "Starting Amal's      %s.\n" , developerName  ) ;

    if( argc < 5 )
    {
        printf("\nMissing command-line file descriptors: %s <getFr. KDC> <sendTo KDC> "
               "<getFr. Basim> <sendTo Basim>\n\n" , argv[0]) ;
        exit(-1) ;
    }
    fd_K2A    = atoi(argv[1]) ;  // Read from KDC    File Descriptor
    fd_A2K    = atoi(argv[2]) ;  // Send to   KDC    File Descriptor
    fd_B2A    = atoi(argv[3]) ;  // Read from Basim  File Descriptor
    fd_A2B    = atoi(argv[4]) ;  // Send to   Basim  File Descriptor

    log = fopen("amal/logAmal.txt" , "w" );
    if( ! log )
    {
        fprintf( stderr , "\nAmal's  %s. Could not create my log file\n" , developerName  ) ;
        exit(-1) ;
    }

    BANNER( log ) ;
    fprintf( log , "Starting Amal\n" ) ;
    BANNER( log ) ;

    fprintf( log , "\n<readFrom KDC> FD=%d , <sendTo KDC> FD=%d , "
                   "<readFrom Basim> FD=%d , <sendTo Basim> FD=%d\n\n" ,
                   fd_K2A , fd_A2K , fd_B2A , fd_A2B );

    // Get Amal's master key with the KDC
    myKey_t  Ka ;  // Amal's master key with the KDC


    // Use  getKeyFromFile( "amal/amalKey.bin" , .... ) )
    if (! getKeyFromFile( "amal/amalKey.bin", &Ka))
    {
        fprintf( stderr, "\nCould not get Amal's Master Key & IV.\n");
        fprintf( log, "\nCould not get Amal's Master Key & IV.\n");
        exit(-1);
    }

    fprintf(log, "Amal has this Master Ka { key , IV }\n");
    BIO_dump_indent_fp( log, Ka.key, SYMMETRIC_KEY_LEN, 4);
	// On failure, print "\nCould not get Amal's Masker key & IV.\n" to both  stderr and the Log file
	// and exit(-1)
	// On success, print "Amal has this Master Ka { key , IV }\n" to the Log file
	// BIO_dump the Key IV indented 4 spaces to the righ
    fprintf( log , "\n" );
    BIO_dump_indent_fp( log, Ka.iv, INITVECTOR_LEN, 4);
    fprintf( log, "\n");

	// BIO_dump the IV indented 4 spaces to the righ


    // Get Amal's pre-created Nonces: Na and Na2
	Nonce_t   Na , Na2;
    fprintf( log , "Amal will use these Nonces:  Na  and Na2\n"  ) ;
	// Use getNonce4Amal () to get Amal's 1st and second nonces into Na and Na2, respectively
    getNonce4Amal( 1, Na);
	// BIO_dump Na indented 4 spaces to the righ
    BIO_dump_indent_fp( log, Na, NONCELEN, 4);

    getNonce4Amal( 2, Na2);


    fprintf( log , "\n" );
    BIO_dump_indent_fp( log, Na2, NONCELEN, 4);

	// BIO_dump Na2 indented 4 spaces to the righ
    fprintf( log , "\n") ;

    fflush( log ) ;

    //*************************************
    // Construct & Send    Message 1
    //*************************************
    BANNER( log ) ;
    fprintf( log , "         MSG1 New\n");
    BANNER( log ) ;

    char *IDa = "Amal is Hope", *IDb = "Basim is Smiley" ;
    size_t  LenMsg1 ;
    uint8_t  *msg1 ;
    LenMsg1 = MSG1_new( log , &msg1 , IDa , IDb , Na ) ;

    if (write(fd_A2K, msg1, LenMsg1) != LenMsg1)
    {
        fprintf( log, "Unable to send all MSG1 bytes to the KDC...");
        fclose( log );
        exit(-1);
    }

    // Send MSG1 to KDC via the appropriate pipe

   fprintf( log , "Amal sent message 1 ( %lu bytes ) to the KDC with:\n    "
                   "IDa ='%s'\n    "
                   "IDb = '%s'\n" , LenMsg1 , IDa , IDb ) ;
    fprintf( log , "    Na ( %lu Bytes ) is:\n" , NONCELEN ) ;
    BIO_dump_indent_fp(log, Na, NONCELEN, 4);
    fprintf( log , "\n") ;

    // BIO_dump the nonce Na
    fflush( log ) ;

    free(msg1);
    // Deallocate any memory allocated for msg1


    // PA-04 Part Two
    // will go here


    //*************************************
    // Final Clean-Up
    //*************************************

    fprintf( log , "\nAmal has terminated normally. Goodbye\n" ) ;
    fclose( log ) ;
    return 0 ;
}
