#include "Gen_RPC8.h"


/*

int passed = 0;

struct f1struct {
    char a;
    enum {
        FIRST = 144,
        SECOND = 999,
    } s;
};

struct f2struct {
    long long x[5];
};

void func1( unsigned a, f1struct f1, unsigned b, void* lastParam ) {
    bool rc = (
        a == 22
        && reinterpret_cast<void*>( 997722  ) == lastParam
        && f1.a == 65
        && f1.s == f1struct::FIRST
        && b == 775
    );
    passed += rc;
    printf( "%32s\t%s\n", "int & int struct:", rc ? "passed" : "failed" );
}


void func2( void* thisPtr, f1struct f1, unsigned a, unsigned b, short c, char e, void* lastParam ) {
    bool rc = (
        thisPtr == func1
        && a == 22
        && reinterpret_cast<void*>( 997722  ) == lastParam
        && f1.a == 65
        && f1.s == f1struct::FIRST
        && b == 775
        && c == -5
        && e == 'A'
    );
    passed += rc;
    printf( "%32s\t%s\n", "mixed size int & int struct:", rc ? "passed" : "failed" );
}

void func3( void* thisPtr, f2struct f2, void* lastParam ) {
    bool rc = (
        thisPtr == func1
        && reinterpret_cast<void*>( 997722  ) == lastParam
        && f2.x[0] == 0x10
        && f2.x[1] == 0x100
        && f2.x[2] == 0x1000
        && f2.x[3] == 0x10000
        && f2.x[4] == 0x100000
    );
    passed += rc;
    // printf( " %lx %lx %lx %lx %lx [%x %x] [%x/%d]\n", f2.x[0], f2.x[1], f2.x[2], f2.x[3], f2.x[4], thisPtr, func1, lastParam, lastParam);
    printf( "%32s\t%s\n", "big struct:", rc ? "passed" : "failed" );
}

void func4( double d, void* lastParam ) {
    bool rc = (
        reinterpret_cast<void*>( 997722  ) == lastParam
        && d == 2.5
    );
    passed += rc;
    printf( "%32s\t%s\n", "1xdouble:", rc ? "passed" : "failed" );
}

void func5( double a, float b, long double c, double d, double e, void* lastParam ) {
    bool rc = (
        reinterpret_cast<void*>( 997722  ) == lastParam
        && a == 2.5
        && b == 2.5
        && c == 2.5
        && d == 4.125E10
        && e == 8.75E-10
    );
    passed += rc;
    printf( "%32s\t%s\n", "mixed doubles:", rc ? "passed" : "failed" );
}

void func6( void* thisPtr, f1struct* f1, void* lastParam ) {
    bool rc = (
        thisPtr == func1
        && reinterpret_cast<void*>( 997722  ) == lastParam
        && f1->a == 65
        && f1->s == f1struct::FIRST
    );
    passed += rc;
    // printf( "%x == %x %d == %d %d == %d %d == %d\n", thisPtr, func1, lastParam, 997722, f1->a, 65, f1->s, f1struct::FIRST );
    printf( "%32s\t%s\n", "struct by ref:", rc ? "passed" : "failed" );
}

*/

bool isReal( double )      { return 1; }
bool isReal( float )       { return 1; }
bool isReal( long double ) { return 1; }

template <class item>
bool isReal(item) { return 0; }

template <class item>
bool isPtr( item* )        { return 1; }

template <class item>
bool isPtr( item )         { return 0; }

template <class item>
void push( char*& p, item const i ) {
    memcpy( (void*)p, (void*)&i, sizeof( i ) );
    p += sizeof( i );
}
template <class item>
void push( char*& p, item*const i ) {
    memcpy( (void*)p, (void*)i, sizeof( *i ) );
    p += sizeof( *i );
}

template <class item>
void push( char*& p, item const*const i ) {
    memcpy( (void*)p, (void*)i, sizeof( *i ) );
    p += sizeof( *i );
}

template <class item>
size_t d_size( item const ) { return sizeof( item ); }

template <class item>
size_t d_size( item const*const ) { return sizeof( item ); }

template <class item>
size_t d_size( item*const ) { return sizeof( item ); }

template <class item>
void push_header( char*& p, item const i ) {
    unsigned int   s = d_size( i );
    unsigned char  f =
                        ( isReal( i ) << AutoRPC::REAL_PARAM_SC )
                        |
                        ( isPtr( i ) << AutoRPC::REF_PARAM_SC  )
                      ;
    push( p, s );
    push( p, f );
}


// typedef AutoRPC::NaturalWord NaturalWord;

/*
int _cf = 0;
void cf( const char* s, int n) {
   printf( ( (_cf = ((_cf + 1) & 1)) ? "%24s = %2d" : "\t%24s = %2d\n"), s, n );
}

int main() {
    printf("conf:\n");
    cf( "ABI", ABI );
    cf( "AUTO_RPC_ALLOC_SEPARATE_FLOATS", AUTO_RPC_ALLOC_SEPARATE_FLOATS );
    #ifdef NO_ASM
    cf( "ASM", 0 );
    #else
    cf( "ASM", 1 );
    #endif
    cf( "AUTO_RPC_AUTORPC_WORD", AUTO_RPC_AUTORPC_WORD );
    cf( "AUTO_RPC_CREATE_FLOAT_MAP", AUTO_RPC_CREATE_FLOAT_MAP );
    cf( "AUTO_RPC_FLOAT_REG_PARAMS", AUTO_RPC_FLOAT_REG_PARAMS );
    cf( "AUTO_RPC_INT_REG_PARAMS", AUTO_RPC_INT_REG_PARAMS );
    cf( "AUTO_RPC_INT_SHADOW_OF_FLOATS", AUTO_RPC_INT_SHADOW_OF_FLOATS );
    cf( "sizeof(long double)", sizeof( long double ) );
    cf( "sizeof(long int)", sizeof( long int ) );
    printf("\n\ntests:\n");
    AutoRPC::CallParams call;
    char buffer[256];
    buffer[0] = 3;
    char* p = &buffer[1];
    unsigned a = 22;
    unsigned b = 775;
    f1struct f1 = { 65, f1struct::FIRST };
    push_header( p, a );
    push_header( p, f1  );
    push_header( p, b  );
    push( p, a );
    push( p, f1 );
    push( p, b );
    if (!AutoRPC::DeserializeParametersAndBuildCall( call, buffer, p - buffer, reinterpret_cast<void*>(997722), 0 ) ) {
        printf( "deserialize1 failed");
        return 1;
    }
    if (!AutoRPC::CallWithStack( call, (void*)func1 )) {
        printf( "call 1 failed");
        return 1;
    }
    short s = -5;
    char e = 'A';

    buffer[0] = 5;
    p = &buffer[1];
    push_header( p, f1 );
    push_header( p, a );
    push_header( p, b );
    push_header( p, s );
    push_header( p, e );

    push( p, f1 );
    push( p, a );
    push( p, b );
    push( p, s );
    push( p, e );
    if (!AutoRPC::DeserializeParametersAndBuildCall( call, buffer, p - buffer, reinterpret_cast<void*>(997722), (void*)func1 ) ) {
        printf( "deserialize2 failed");
        return 1;
    }
    if (!AutoRPC::CallWithStack( call, (void*)func2 )) {
        printf( "call2 failed");
        return 1;
    }

    buffer[0] = 1;
    p = &buffer[1];
    f2struct f2 = { 0x10, 0x100, 0x1000, 0x10000, 0x100000 };
    push_header( p, f2 );
    push( p, f2 );

    if (!AutoRPC::DeserializeParametersAndBuildCall( call, buffer, p - buffer, reinterpret_cast<void*>(997722), (void*)func1 ) ) {
        printf( "deserialize3 failed");
        return 1;
    }
    if (!AutoRPC::CallWithStack( call, (void*)func3 )) {
        printf( "call3 failed");
        return 1;
    }

    buffer[0] = 1;
    p = &buffer[1];
    double d = 2.5;
    push_header( p, d );
    push( p, d );
    if (!AutoRPC::DeserializeParametersAndBuildCall( call, buffer, p - buffer, reinterpret_cast<void*>(997722), (void*)0 ) ) {
        printf( "deserialize4 failed");
        return 1;
    }
    if (!AutoRPC::CallWithStack( call, (void*)func4 )) {
        printf( "call4 failed");
        return 1;
    }

    buffer[0] = 5;
    p = &buffer[1];
    float rb = 2.5;
    long double rc = 2.5;
    double rd = 4.125E10;
    double re = 8.75E-10;
    push_header( p, d );
    push_header( p, rb );
    push_header( p, rc );
    push_header( p, rd );
    push_header( p, re );
    push( p, d );
    push( p, rb );
    push( p, rc );
    push( p, rd );
    push( p, re );
    if (!AutoRPC::DeserializeParametersAndBuildCall( call, buffer, p - buffer, reinterpret_cast<void*>(997722), (void*)0 ) ) {
        printf( "deserialize5 failed");
        return 1;
    }
    if (!AutoRPC::CallWithStack( call, (void*)func5 )) {
        printf( "call5 failed");
        return 1;
    }
    buffer[0] = 1;
    p = &buffer[1];
    push_header( p, &f1 );
    push( p, &f1 );
    if (!AutoRPC::DeserializeParametersAndBuildCall( call, buffer, p - buffer, reinterpret_cast<void*>(997722), (void*)func1 ) ) {
        printf( "deserialize6 failed");
        return 1;
    }

    if (!AutoRPC::CallWithStack( call, (void*)func6 )) {
        printf( "call6 failed");
        return 1;
    }

    return passed;
}
*/

