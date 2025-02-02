#ifndef LIBNAME
#error Meh
#endif

#define FUNC3(M,N) wrapped##M##N
#define FUNC2(M,N) FUNC3(M,N)
#define FUNC(N) FUNC2(LIBNAME,N)
#define QUOTE(M) #M
#define PRIVATE2(P) QUOTE(wrapped##P##_private.h)
#define PRIVATE(P) PRIVATE2(P)
#define MAPNAME3(N,M) N##M
#define MAPNAME2(N,M) MAPNAME3(N,M)
#define MAPNAME(N) MAPNAME2(LIBNAME,N)

// prepare the maps
#define _DOIT(P,Q) QUOTE(generated/wrapped##P##Q.h)
#define DOIT(P,Q) _DOIT(P,Q)
#include DOIT(LIBNAME,defs)

// regular symbol mapped to itself
#define GO(N, W)
// regular symbol mapped to itself, but weak
#define GOW(N, W)
// symbol mapped to my_symbol
#define GOM(N, W)
// symbol mapped to my_symbol, but weak
#define GOWM(N, W)
// regular symbol mapped to itself, that returns a structure
#define GOS(N, W)
// weak symbol mapped to itself, that returns a structure
#define GOWS(N, W)
// symbol mapped to another one
#define GO2(N, W, O)
// weak symbol mapped to another one
#define GOW2(N, W, O)
// data
#define DATA(N, S)
// data, Weak (type V)
#define DATAV(N, S)
// data, Uninitialized (type B)
#define DATAB(N, S)
// data, "my_" type
#define DATAM(N, S)

// #define the 4 maps first
#undef GO
#define GO(N, W) {#N, W, 0},
#undef GOW
#define GOW(N, W) {#N, W, 1},
static const map_onesymbol_t MAPNAME(symbolmap)[] = {
    #include PRIVATE(LIBNAME)
};
#undef GO
#define GO(N, W)
#undef GOW
#define GOW(N, W)
#undef GOM
#define GOM(N, W) {#N, W, 0},
#undef GOWM
#define GOWM(N, W) {#N, W, 1},
static const map_onesymbol_t MAPNAME(mysymbolmap)[] = {
    #include PRIVATE(LIBNAME)
};
#undef GOM
#define GOM(N, W)
#undef GOWM
#define GOWM(N, W)
#undef GOS
#define GOS(N, W) {#N, W, 0},
#undef GOWS
#define GOWS(N, W) {#N, W, 1},
static const map_onesymbol_t MAPNAME(stsymbolmap)[] = {
    #include PRIVATE(LIBNAME)
};
#undef GOS
#define GOS(N, W)
#undef GOWS
#define GOWS(N, W)
#undef GO2
#define GO2(N, W, O) {#N, W, 0, #O},
#undef GOW2
#define GOW2(N, W, O) {#N, W, 1, #O},
static const map_onesymbol2_t MAPNAME(symbol2map)[] = {
    #include PRIVATE(LIBNAME)
};
#undef GO2
#define GO2(N, W, O)
#undef GOW2
#define GOW2(N, W, O)
#undef DATA
#undef DATAV
#undef DATAB
#define DATA(N, S) {#N, S, 0},
#define DATAV(N, S) {#N, S, 1},
#define DATAB(N, S) {#N, S, 0},
static const map_onedata_t MAPNAME(datamap)[] = {
    #include PRIVATE(LIBNAME)
};
#undef DATA
#undef DATAV
#undef DATAB
#define DATA(N, S)
#define DATAV(N, S)
#define DATAB(N, S)
#undef DATAM
#define DATAM(N, S) {#N, S, 0},
static const map_onedata_t MAPNAME(mydatamap)[] = {
    #include PRIVATE(LIBNAME)
};

#undef GO
#undef GOW
#undef GOM
#undef GOWM
#undef GOS
#undef GOWS
#undef GO2
#undef GOW2
#undef DATA
#undef DATAV
#undef DATAB
#undef DATAM

#include DOIT(LIBNAME,undefs)
#undef DOIT
#undef _DOIT

void NativeLib_CommonInit(library_t* lib);
int FUNC(_init)(library_t* lib, box86context_t* box86)
{
// Init first
    free(lib->path); lib->path=NULL;
#ifdef PRE_INIT
    PRE_INIT
#endif
    {
        lib->priv.w.lib = dlopen(MAPNAME(Name), RTLD_LAZY | RTLD_GLOBAL);
        if(!lib->priv.w.lib) {
#ifdef ALTNAME
        lib->priv.w.lib = dlopen(ALTNAME, RTLD_LAZY | RTLD_GLOBAL);
        if(!lib->priv.w.lib)
#endif
#ifdef ALTNAME2
            {
            lib->priv.w.lib = dlopen(ALTNAME2, RTLD_LAZY | RTLD_GLOBAL);
            if(!lib->priv.w.lib)
#endif
                return -1;
#ifdef ALTNAME2
                else lib->path = strdup(ALTNAME2);
            } else lib->path = strdup(ALTNAME);
#endif
        } else lib->path = strdup(MAPNAME(Name));
    }
    NativeLib_CommonInit(lib);

    khint_t k;
    int ret;
    int cnt;

    // populates maps...
#define DOIT(mapname) \
    cnt = sizeof(MAPNAME(mapname))/sizeof(map_onesymbol_t);                         \
    for (int i = 0; i < cnt; ++i) {                                                 \
        if (MAPNAME(mapname)[i].weak) {                                             \
            k = kh_put(symbolmap, lib->w##mapname, MAPNAME(mapname)[i].name, &ret); \
            kh_value(lib->w##mapname, k) = MAPNAME(mapname)[i].w;                   \
        } else {                                                                    \
            k = kh_put(symbolmap, lib->mapname, MAPNAME(mapname)[i].name, &ret);    \
            kh_value(lib->mapname, k) = MAPNAME(mapname)[i].w;                      \
        }                                                                           \
        if (strchr(MAPNAME(mapname)[i].name, '@'))                                  \
            AddDictionnary(box86->versym, MAPNAME(mapname)[i].name);                \
	}
	DOIT(symbolmap)
	DOIT(mysymbolmap)
	DOIT(stsymbolmap)
#undef DOIT
    cnt = sizeof(MAPNAME(symbol2map))/sizeof(map_onesymbol2_t);
    for (int i=0; i<cnt; ++i) {
        k = kh_put(symbol2map, lib->symbol2map, MAPNAME(symbol2map)[i].name, &ret);
        kh_value(lib->symbol2map, k).name = MAPNAME(symbol2map)[i].name2;
        kh_value(lib->symbol2map, k).w = MAPNAME(symbol2map)[i].w;
        kh_value(lib->symbol2map, k).weak = MAPNAME(symbol2map)[i].weak;
        if(strchr(MAPNAME(symbol2map)[i].name, '@'))
            AddDictionnary(box86->versym, MAPNAME(symbol2map)[i].name);
    }
    cnt = sizeof(MAPNAME(datamap))/sizeof(map_onedata_t);
    for (int i=0; i<cnt; ++i) {
        if(MAPNAME(datamap)[i].weak) {
            k = kh_put(datamap, lib->wdatamap, MAPNAME(datamap)[i].name, &ret);
            kh_value(lib->wdatamap, k) = MAPNAME(datamap)[i].sz;
        } else {
            k = kh_put(datamap, lib->datamap, MAPNAME(datamap)[i].name, &ret);
            kh_value(lib->datamap, k) = MAPNAME(datamap)[i].sz;
        }
    }
    cnt = sizeof(MAPNAME(mydatamap))/sizeof(map_onedata_t);
    for (int i=0; i<cnt; ++i) {
        k = kh_put(datamap, lib->mydatamap, MAPNAME(mydatamap)[i].name, &ret);
        kh_value(lib->mydatamap, k) = MAPNAME(mydatamap)[i].sz;
    }
#ifdef CUSTOM_INIT
    CUSTOM_INIT
#endif
    
    return 0;
}

void NativeLib_FinishFini(library_t* lib);
void FUNC(_fini)(library_t* lib)
{
#ifdef CUSTOM_FINI
    CUSTOM_FINI
#endif
    return NativeLib_FinishFini(lib);
}

int NativeLib_defget(library_t* lib, const char* name, uintptr_t *offs, uint32_t *sz, int version, const char* vername, int local);
int FUNC(_get)(library_t* lib, const char* name, uintptr_t *offs, uint32_t *sz, int version, const char* vername, int local)
{
#ifdef CUSTOM_FAIL
    uintptr_t addr = 0;
    uint32_t size = 0;
    void* symbol = NULL;
    if (!getSymbolInMaps(lib, name, 0, &addr, &size, version, vername, local)) {
        CUSTOM_FAIL
    }
    if(!addr && !size)
        return 0;
    *offs = addr;
    *sz = size;
    return 1;
#else
    return NativeLib_defget(lib, name, offs, sz, version, vername, local);
#endif
}

int NativeLib_defgetnoweak(library_t* lib, const char* name, uintptr_t *offs, uint32_t *sz, int version, const char* vername, int local);
int FUNC(_getnoweak)(library_t* lib, const char* name, uintptr_t *offs, uint32_t *sz, int version, const char* vername, int local)
{
#ifdef CUSTOM_FAIL
    uintptr_t addr = 0;
    uint32_t size = 0;
    void* symbol = NULL;
    if (!getSymbolInMaps(lib, name, 1, &addr, &size, version, vername, local)) {
        CUSTOM_FAIL
    }
    if(!addr && !size)
        return 0;
    *offs = addr;
    *sz = size;
    return 1;
#else
    return NativeLib_defgetnoweak(lib, name, offs, sz, version, vername, local);
#endif
}
