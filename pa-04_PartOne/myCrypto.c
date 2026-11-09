/*----------------------------------------------------------------------------
My Cryptographic Library

FILE:   myCrypto.c

Written By:  Team #10
     1- Danny Ramos
     2- Samuel George

Submitted on:
----------------------------------------------------------------------------*/

#include "myCrypto.h"

//***********************************************************************
// pLAB-01
//***********************************************************************

void handleErrors(char *msg)
{
    fprintf(stderr, "\n%s\n", msg);
    ERR_print_errors_fp(stderr);
    exit(-1);
}

//------------------------------------------------------------------------
// Encrypt the plain text stored at 'pPlainText' into the
// caller-allocated memory at 'pCipherText'
// Caller must allocate sufficient memory for the cipher text
// Returns size of the cipher text in bytes

unsigned encrypt(uint8_t *pPlainText, unsigned plainText_len,
                 const uint8_t *key, const uint8_t *iv, uint8_t *pCipherText)
{
    int status;
    unsigned len = 0, encryptedLen = 0;

    /* Create and initialise the context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        handleErrors("encrypt: failed to create CTX");

    // Initialise the encryption operation.
    status = EVP_EncryptInit_ex(ctx, ALGORITHM(), NULL, key, iv);
    if (status != 1)
        handleErrors("encrypt: failed to EncryptInit_ex");

    // Call EncryptUpdate as many times as needed (e.g. inside a loop)
    // to perform regular encryption
    status = EVP_EncryptUpdate(ctx, pCipherText, &len, pPlainText, plainText_len);
    if (status != 1)
        handleErrors("encrypt: failed to EncryptUpdate");
    encryptedLen += len;

    // If additional ciphertext may still be generated,
    // the pCipherText pointer must be first advanced forward
    pCipherText += len;

    // Finalize the encryption.
    status = EVP_EncryptFinal_ex(ctx, pCipherText, &len);
    if (status != 1)
        handleErrors("encrypt: failed to EncryptFinal_ex");
    encryptedLen += len; // len could be 0 if no additional cipher text was generated

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return encryptedLen;
}

//------------------------------------------------------------------------
// Decrypt the cipher text stored at 'pCipherText' into the
// caller-allocated memory at 'pDecryptedText'
// Caller must allocate sufficient memory for the decrypted text
// Returns size of the decrypted text in bytes

unsigned decrypt(uint8_t *pCipherText, unsigned cipherText_len,
                 const uint8_t *key, const uint8_t *iv, uint8_t *pDecryptedText)
{
    int status;
    unsigned len = 0, decryptedLen = 0;

    /* Create and initialise the context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        handleErrors("decrypt: failed to create CTX");

    // Initialise the decryption operation.
    status = EVP_DecryptInit_ex(ctx, ALGORITHM(), NULL, key, iv);
    if (status != 1)
        handleErrors("decrypt: failed to DecryptInit_ex");

    // Call DecryptUpdate as many times as needed (e.g. inside a loop)
    // to perform regular decryption
    status = EVP_DecryptUpdate(ctx, pDecryptedText, &len, pCipherText, cipherText_len);
    if (status != 1)
        handleErrors("decrypt: failed to DecryptUpdate");
    decryptedLen += len;

    // If additional decrypted text may still be generated,
    // the pDecryptedText pointer must be first advanced forward
    pDecryptedText += len;

    // Finalize the decryption.
    status = EVP_DecryptFinal_ex(ctx, pDecryptedText, &len);
    if (status != 1)
        handleErrors("decrypt: failed to DecryptFinal_ex");
    decryptedLen += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return decryptedLen;
}

//***********************************************************************
// PA-01
//***********************************************************************

static unsigned char plaintext[PLAINTEXT_LEN_MAX],
    ciphertext[CIPHER_LEN_MAX],
    decryptext[DECRYPTED_LEN_MAX];

// above arrays being static to resolve runtime stack size issue.
// However, that makes the code non-reentrant for multithreaded application
//-----------------------------------------------------------------------------

int encryptFile(int fd_in, int fd_out, const uint8_t *key, const uint8_t *iv)
{

    int n, len, status = 0, total = 0;

    /* Create and initialise the context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        handleErrors("encryptFile: failed to create CTX");

    /* Initialise the encryption operation */
    status = EVP_EncryptInit_ex(ctx, ALGORITHM(), NULL, key, iv);
    if (status != 1)
        handleErrors("encryptFile: failed to EncryptInit_ex");

    /* Loop through the file until we reach the end */
    while ((n = read(fd_in, plaintext, PLAINTEXT_LEN_MAX)) > 0)
    {
        /*
            Call EncryptUpdate as many times as needed (e.g. inside a loop)
            to perform regular encryption
        */
        status = EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, n);
        if (status != 1)
            handleErrors("encryptFile: failed to EncryptUpdate");

        /*
            Write the ciphertext to the file descriptor out fd_out
        */
        if (write(fd_out, ciphertext, len) != len)
        {
            handleErrors("encryptFile: failed to write ciphertext");
        }
        total += len;
    }

    if (n < 0)
    {
        handleErrors("encryptFile: failed to read plaintext");
    }

    /* Finalize the Encryption */
    status = EVP_EncryptFinal_ex(ctx, ciphertext, &len);
    if (status != 1)
        handleErrors("encryptFile: failed to EncryptFinal_ex");

    /* Write the final ciphertext to fd_out */
    if (write(fd_out, ciphertext, len) != len)
        handleErrors("encryptFile: failed to write final ciphertext");
    total += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return total;
}

//-----------------------------------------------------------------------------
int decryptFile(int fd_in, int fd_out, const uint8_t *key, const uint8_t *iv)
{

    int n, len, status = 0, total = 0;

    /* Create and initialise the context */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        handleErrors("decryptFile: failed to create CTX");

    /* Initialise the decryption operation */
    status = EVP_DecryptInit_ex(ctx, ALGORITHM(), NULL, key, iv);
    if (status != 1)
        handleErrors("decryptFile: failed to DecryptInit_ex");

    /* Loop through the file until we reach the end */
    while ((n = read(fd_in, ciphertext, CIPHER_LEN_MAX)) > 0)
    {
        /*
            Call DecryptUpdate as many times as needed (e.g. inside a loop)
            to perform regular decryption
        */
        status = EVP_DecryptUpdate(ctx, decryptext, &len, ciphertext, n);
        if (status != 1)
            handleErrors("decryptFile: failed to DecryptUpdate");

        /*
            Write the decrypted text to the file descriptor out fd_out
        */
        if (write(fd_out, decryptext, len) != len)
        {
            handleErrors("decryptFile: failed to write plaintext");
        }
        total += len;
    }
    if (n < 0)
        handleErrors("decryptFile: failed to read ciphertext");

    /* Finalize the Decryption */
    status = EVP_DecryptFinal_ex(ctx, decryptext, &len);
    if (status != 1)
        handleErrors("decryptFile: failed to DecryptFinal_ex");

    /* Write the final plaintext to fd_out */
    if (write(fd_out, decryptext, len) != len)
        handleErrors("decryptFile: failed to write final plaintext");
    total += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return total;
}

//***********************************************************************
// pLAB-02
//***********************************************************************

EVP_PKEY *getRSAfromFile(char *filename, int public)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "getRSAfromFile: Unable to open RSA key file %s \n", filename);
        return NULL;
    }

    EVP_PKEY *key = EVP_PKEY_new();
    if (public)
        key = PEM_read_PUBKEY(fp, &key, NULL, NULL);
    else
        key = PEM_read_PrivateKey(fp, &key, NULL, NULL);

    fclose(fp);

    return key;
}

//***********************************************************************
// PA-02
//***********************************************************************
// Sign the 'inData' array into the 'sig' array using the private 'privKey'
// 'inLen' is the size of the input array in bytes.
// the '*sig' pointer will be allocated memory large enough to store the signature
// report the actual length in bytes of the result in 'sigLen'
//
// Returns:
//    1 on success, or 0 on ANY REASON OF FAILURE

int privKeySign(uint8_t **sig, size_t *sigLen, EVP_PKEY *privKey,
                uint8_t *inData, size_t inLen)
{
    // Guard against incoming NULL pointers
    if (!sig || !sigLen || !privKey || !inData || !inLen)
        return 0;

    // Create and Initialize a context for RSA private-key signing
    EVP_PKEY_CTX *ctx;
    ctx = EVP_PKEY_CTX_new( privKey, NULL );
    if (!ctx)
        return 0;

    // EVP_PKEY_sign_init( )
    if (EVP_PKEY_sign_init(ctx) <= 0)
    {
        printf( "privKeySign: PKEY init failed\n" );
        EVP_PKEY_CTX_free( ctx );
        return 0;
    }

    // EVP_PKEY_CTX_set_rsa_padding( use RSA_PKCS1_PADDING )
    if (EVP_PKEY_CTX_set_rsa_padding( ctx, RSA_PKCS1_PADDING ) <= 0)
    {
        printf( "privKeySign: rsa padding failed\n" );
        EVP_PKEY_CTX_free( ctx );
        return 0;
    }

    // Determine how big the size of the signature could be
    size_t cipherLen;
    if ( EVP_PKEY_sign( ctx, NULL, &cipherLen, inData, inLen ) <= 0 )
    {
        printf( "privKeySign: sign for size failed\n" );
        EVP_PKEY_CTX_free( ctx );
        return 0;
    }

    // Next allocate memory for the signature
    *sig = malloc( cipherLen );
    if (!*sig)
    {
        printf( "privKeySign: *sig malloc failed\n" );
        EVP_PKEY_CTX_free( ctx );
        return 0;
    }

    // Now, actually sign the inData using EVP_PKEY_sign( )
    if ( EVP_PKEY_sign( ctx, *sig, &cipherLen, inData, inLen ) <= 0 )
    {
        printf( "privKeySign: sign for actual sign failed\n" );
        free( *sig );
        *sig = NULL;
        EVP_PKEY_CTX_free( ctx );
        return 0;
    }

    *sigLen = cipherLen;

    // All is good
    EVP_PKEY_CTX_free(ctx); // remember to do this if any failure is encountered above

    return 1;
}

//-----------------------------------------------------------------------------
// Verify that the provided signature in 'sig' when decrypted using 'pubKey'
// matches the data in 'data'
// Returns 1 if they match, 0 otherwise

int pubKeyVerify(uint8_t *sig, size_t sigLen, EVP_PKEY *pubKey, uint8_t *data, size_t dataLen)
{
    int status = 0;

    // Guard against incoming NULL pointers
    if (!sig || !pubKey || !data)
    {
        printf("\n******* pkeySign received some NULL pointers\n");
        return 0;
    }

    // Create and Initialize a context for RSA public-key signature verification
    EVP_PKEY_CTX *ctx;
    ctx = EVP_PKEY_CTX_new(pubKey, NULL);
    if (!ctx)
    {
        printf("pubKeyVerify: failed to create EVP_PKEY_CTX\n");
        return 0;
    }

    // EVP_PKEY_verify_init( )
    status = EVP_PKEY_verify_init(ctx);
    if (status != 1)
    {
        printf("pubKeyVerify: failed to EVP_PKEY_verify_init\n");
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // EVP_PKEY_CTX_set_rsa_padding( use RSA_PKCS1_PADDING )
    status = EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING);
    if (status != 1)
    {
        printf("pubKeyVerify: failed to EVP_PKEY_CTX_set_rsa_padding\n");
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }

    // Verify the signature vs the incoming data using this context
    int decision = EVP_PKEY_verify(ctx, sig, sigLen, data, dataLen);

    //  free any dynamically-allocated objects
    EVP_PKEY_CTX_free(ctx);

    return decision == 1;
}

//-----------------------------------------------------------------------------

size_t fileDigest(int fd_in, int fd_out, uint8_t *digest)
// Read all the incoming data stream from the 'fd_in' file descriptor
// Apply the HASH_ALGORITHM() to compute the hash value of this incoming data into the array 'digest'
// If the file descriptor 'fd_out' is > 0, also write a copy of the incoming data stream file to 'fd_out'
// Returns actual size in bytes of the computed digest

// ** Follows similar structure to encryptFile **
{
    EVP_MD_CTX *mdCtx;
    ssize_t nBytes;
    unsigned int mdLen;
    int status = 0;

    // Use EVP_MD_CTX_new() to create new hashing context
    mdCtx = EVP_MD_CTX_new();
    if (!mdCtx)
        handleErrors("fileDigest: failed to create mdCTX");

    // Initialize the context using EVP_DigestInit() so that it deploys
    // the HASH_ALGORITHM() hashing function
    status = EVP_DigestInit(mdCtx, HASH_ALGORITHM());
    if (status != 1)
        handleErrors("fileDigest: failed to EVP_DigestInit");

    while ((nBytes = read(fd_in, plaintext, PLAINTEXT_LEN_MAX)) > 0) // Loop until end-of input file
    {
        // Read a chunk of input from fd_in. Exit the loop when End-of-File is reached
        status = EVP_DigestUpdate(mdCtx, plaintext, nBytes);
        if (status != 1)
            handleErrors("fileDigest: failed to EVP_DigestUpdate");

        // if ( fd_out > 0 ) send the above chunk of data to fd_out
        if (fd_out > 0)
        {
            if (write(fd_out, plaintext, nBytes) != nBytes)
                handleErrors("fileDigest: failed to write to fd_out");
        }
    }

    if (nBytes < 0)
    {
        handleErrors("fileDigest: failed to read plaintext");
    }

    status = EVP_DigestFinal(mdCtx, digest, &mdLen);
    if (status != 1)
        handleErrors("fileDigest: failed to EVP_DigestFinal");

    EVP_MD_CTX_destroy(mdCtx);
    return mdLen;
}

//***********************************************************************
// PA-04  Part  One
//***********************************************************************

void exitError( char *errText )
{
    fprintf( stderr , "%s\n" , errText ) ;
    exit(-1) ;
}

//-----------------------------------------------------------------------------
// Utility to read Key/IV from a file
// Return:  1 on success, or 0 on failure

int getKeyFromFile( char *keyF , myKey_t *x )
{
    int   fd_key  ;
    
    fd_key = open( keyF , O_RDONLY )  ;
    if( fd_key == -1 ) 
    { 
        fprintf( stderr , "\nCould not open key file '%s'\n" , keyF ); 
        return 0 ; 
    }

    // first, read the symmetric encryption key
	if( SYMMETRIC_KEY_LEN  != read ( fd_key , x->key , SYMMETRIC_KEY_LEN ) ) 
    { 
        fprintf( stderr , "\nCould not read key from file '%s'\n" , keyF ); 
        return 0 ; 
    }

    // Next, read the Initialialzation Vector
    if ( INITVECTOR_LEN  != read ( fd_key , x->iv , INITVECTOR_LEN ) ) 
    { 
        fprintf( stderr , "\nCould not read the IV from file '%s'\n" , keyF ); 
        return 0 ; 
    }
	
    close( fd_key ) ;
    
    return 1;  //  success
}

//-----------------------------------------------------------------------------
// Allocate & Build a new Message #1 from Amal to the KDC 
// Where Msg1 is:  Len(A)  ||  A  ||  Len(B)  ||  B  ||  Na
// All Len(*) fields are size_t integers
// Set *msg1 to point at the newly built message
// Msg1 is not encrypted
// Returns the size (in bytes) of Message #1 

size_t MSG1_new ( FILE *log , uint8_t **msg1 , const char *IDa , const char *IDb , const Nonce_t Na )
{

    //  Check agains any NULL pointers in the arguments

    size_t  LenA    = strlen(IDa); //  number of bytes in IDa
    size_t  LenB    = strlen(IDb); //  number of bytes in IDb
    size_t  LenMsg1 = //  number of bytes in the completed MSG1 ;;
    size_t *lenPtr ; 
    uint8_t  *p ;

    // Allocate memory for msg1. MUST always check malloc() did not fail

    // Fill in Msg1:  Len( IDa )  ||  IDa   ||  Len( IDb )  ||  IDb   ||  Na
    p = *msg1;
    
	// use the pointer p to traverse through msg1 and fill the successive parts of the msg 

    fprintf( log , "The following new MSG1 ( %lu bytes ) has been created by MSG1_new ():\n" , LenMsg1 ) ;
    // BIO_dumpt the completed MSG1 indented 4 spaces to the right
    fprintf( log , "\n" ) ;
    
    return LenMsg1 ;
}

//-----------------------------------------------------------------------------
// Receive Message #1 by the KDC from Amal via the pipe's file descriptor 'fd'
// Parse the incoming msg1 into the values IDa, IDb, and Na

void  MSG1_receive( FILE *log , int fd , char **IDa , char **IDb , Nonce_t Na )
{

    //  Check agains any NULL pointers in the arguments

    size_t LenMsg1 = 0, LenA , lenB ;
	// Throughout this function, don't forget to update LenMsg1 as you receive its components
 
    // Read in the components of Msg1:  Len(IDa)  ||  IDa  ||  Len(IDb)  ||  IDb  ||  Na

    // 1) Read Len(ID_A)  from the pipe ... But on failure to read Len(IDa): 
    {
        fprintf( log , "Unable to receive all %lu bytes of Len(IDA) "
                       "in MSG1_receive() ... EXITING\n" , LENSIZE );
        
        fflush( log ) ;  fclose( log ) ;   
        exitError( "Unable to receive all bytes LenA in MSG1_receive()" );
    }

    
    // 2) Allocate memory for ID_A ... But on failure to allocate memory:
    {
        fprintf( log , "Out of Memory allocating %u bytes for IDA in MSG1_receive() "
                       "... EXITING\n" , LenA );
        fflush( log ) ;  fclose( log ) ;
        exitError( "Out of Memory allocating IDA in MSG1_receive()" );
    }

 	// On failure to read ID_A from the pipe
    {
        fprintf( log , "Out of Memory allocating %lu bytes for IDA in MSG1_receive() "
                       "... EXITING\n" , LenA );
        fflush( log ) ;  fclose( log ) ;
        exitError( "Out of Memory allocating IDA in MSG1_receive()" );
    }

    // 3) Read Len( ID_B )  from the pipe    But on failure to read Len( ID_B ):
    {
        fprintf( log , "Unable to receive all %lu bytes of Len(IDB) "
                       "in MSG1_receive() ... EXITING\n" , LENSIZE );
        
        fflush( log ) ;  fclose( log ) ;   
        exitError( "Unable to receive all bytes of LenB in MSG1_receive()" );
    }

    // 4) Allocate memory for ID_B    But on failure to allocate memory:
    {
        fprintf( log , "Out of Memory allocating %u bytes for IDB in MSG1_receive() "
                       "... EXITING\n" , lenB );
        fflush( log ) ;  fclose( log ) ;
        exitError( "Out of Memory allocating IDB in MSG1_receive()" );
    }

 	// Now, read IDb ... But on failure to read ID_B from the pipe
    {
        fprintf( log , "Unable to receive all %u bytes of IDB in MSG1_receive() "
                       "... EXITING\n" , lenB );
        fflush( log ) ;  fclose( log ) ;
        exitError( "Unable to receive all bytes of IDB in MSG1_receive()" );
    }
    
    // 5) Read Na   But on failure to read Na from the pipe
    {
        fprintf( log , "Unable to receive all %lu bytes of Na "
                       "in MSG1_receive() ... EXITING\n" , NONCELEN );
        
        fflush( log ) ;  fclose( log ) ;   
        exitError( "Unable to receive all bytes of Na in MSG1_receive()" );
    }
 
    fprintf( log , "MSG1 ( %lu bytes ) has been received"
                   " on FD %d by MSG1_receive():\n" ,  LenMsg1 , fd  ) ;   
    fflush( log ) ;

    return ;
}
