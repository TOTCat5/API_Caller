#include <windows.h>
#include <stdio.h>
#include <libloaderapi.h>
#include <heapapi.h>



void removeComments(char *str)
{
    bool oneLineComment=false;
    char *dst=str;
    for(char *src=str;*src;++src)
    {
        if(*src=='/'&&*(src+1)=='*')
        {
            while(
                *src&&
                *src=='*'&&
                *(src+1)=='/'
            )
            {
                src++;
            }

            continue;
        }

        if(
            *src=='/'&&
            *(src+1)=='/'
        )
        {
            oneLineComment=true;
        }

        if(*src=='\n')
        {
            oneLineComment=false;
        }

        if(!oneLineComment)
        {
            *(dst++)=*src;
        }
    }

    *dst='\0';
}

void removeDefinitions(char *str)
{
    bool inDefinition=false;
    char *dst=str;
    for(char *src=str;*src;++src)
    {
        if(*src=='#')
        {
            inDefinition=true;
        }
        if(
            *src=='\n'||
            *src=='\r'
        )
        {
            inDefinition=false;
            continue;
        }

        if(!inDefinition)
        {
            *(dst++)=*src;
        }
    }

    *dst='\0';
}

char *preprocess(char *str)
{
    removeComments(str);
    
    removeDefinitions(str);
    
    return str;
}


typedef enum TokenEnum
{
    TokenEnumUnidentified,
    TokenEnumId,
    TokenEnumConstant,
    TokenEnumPonctuation,
    TokenEnumSpace

} TokenEnum;

typedef struct TokenListNode
{
    TokenListNode *next;
    TokenListNode *prev;

    char *str;
    size_t strLen;

    TokenEnum e;
} TokenListNode;


#define C_PONCTUATIONS\
    x("")

TokenListNode *lex(char *code)
{
    TokenListNode *prev=NULL;
    

}


typedef struct HeaderFunction
{

} HeaderFunction;

HeaderFunction getHeaderLineOfProgram(char *buf,size_t bufSize,char *programName,size_t programNameSize)
{
    char *preprocessed=preprocess(strdup(buf));


}

void getNamesIdx(int argc,char *argv[],size_t *pLibraryNameIdx,size_t *pProcessNameIdx,size_t *pHeaderFileNameIdx,bool *pOldBehavior)
{

    size_t libraryNameIdx=-1;
    size_t processNameIdx=-1;
    size_t headerFileNameIdx=-1;

    const char *libraryNameWarningList[]={
        #if defined(WIN32) || defined(__WIN32__) || defined(WIN64) ||defined(__WIN64__)
        "-dll",
        #else
        "-so",
        #endif
        "-l",
        "-lib",
        "-library",
    };
    const char *processNameWarningList[]={
        "-p",
        "-pn",
        "-proc",
        "-processName",
        "-procName",
        "-process_name",
        "-proc_name"
    };
    const char *headerFileNameWarningList[]={
        "-h",
        "-header",
        "-header_def",
        "-def"
    };
    
    bool canTakeNewBehavior=true;
    #define CHECK_ARGUMENTS(idx,list) \
        for(size_t j=0;j<sizeof(list)/sizeof(list[0]);++j)\
        {\
            if(!strcmp(argv[i],list[j]))\
            {\
                if(idx!=-1)\
                {\
                    canTakeNewBehavior=false;\
                }\
                idx=i+1;\
            }\
        }

    for(size_t i=0;i<argc-1;++i)
    {
        CHECK_ARGUMENTS(libraryNameIdx,libraryNameWarningList)
        CHECK_ARGUMENTS(processNameIdx,processNameWarningList)
        CHECK_ARGUMENTS(headerFileNameIdx,headerFileNameWarningList)
    }
    if(canTakeNewBehavior)
    {
        canTakeNewBehavior=libraryNameIdx!=-1&&processNameIdx!=-1;
    }

    #ifdef NDEBUG
    bool oldBehavior=!canTakeNewBehavior;
    #else
    bool oldBehavior=false;
    #endif


    if(oldBehavior)
    {
        printf("error while parsing the arguments. Taking old behavior.\n");
        libraryNameIdx=1;
        processNameIdx=2;
    }

    *pLibraryNameIdx=libraryNameIdx;
    *pProcessNameIdx=processNameIdx;
    *pHeaderFileNameIdx=headerFileNameIdx;
    *pOldBehavior=oldBehavior;
}

int main(int argc,const char *argv[])
{
    for(int i=0;i<argc;++i)
    {
        printf("%s ",argv[i]);
    }
    printf("\n");

    #ifdef NDEBUG
    if(argc<3)
    {
        fprintf(stderr,"You can't have less than 2 arguments. arguments=%d\n",argc-1);
        return -1;
    }
    #endif

    size_t libraryNameIdx=0;
    size_t processNameIdx=0;
    size_t headerFileNameIdx=0;
    bool oldBehavior=false;
    getNamesIdx(
        argc,argv,
        &libraryNameIdx,
        &processNameIdx,
        &headerFileNameIdx,
        &oldBehavior
    );


    #ifdef NDEBUG
    const char *libraryName=argv[libraryNameIdx];
    const char *processName=argv[processNameIdx];
    #else
    const char *libraryName="kernel32.dll";
    const char *processName="GetProcessHeap";
    #endif
    if(headerFileNameIdx==-1)
    {
        
        bool result=true;
        while(true)
        {
            printf("can't find a header/definition file. is that what you want ? [Y/n]:\n");
            char userInput=0;
            scanf("%c",&userInput);
            userInput&=~32;
            if(userInput=='N')
            {
                result=false;
                break;
            }
            if(userInput=='Y')
                break;
        }
        if(!result)
        {
            return -1;
        }
    }
    #ifdef NDEBUG
    const char *headerFileName=argv[headerFileNameIdx];
    #else
    const char *headerFileName="C:\\Program Files (x86)\\Windows Kits\\10\\Include\\10.0.26100.0\\um\\heapapi.h";
    #endif



    HMODULE module=LoadLibraryA(libraryName);
    if(!module)
    {
        fprintf(stderr,"\"%s\" isn't located. Error code:%lu\n",libraryName);
        return -1;
    }

    FARPROC proc=GetProcAddress(module,processName);
    if(!proc)
    {
        fprintf(stderr,"couldn't find \"%s\" in \"%s\". Error code:%lu\n",processName,libraryName,GetLastError());
        return -1;
    }

    FILE *f=fopen(headerFileName,"r");
    if(!f)
    {
        printf("couldn't find \"%s\". Error code:%i\n",headerFileName,errno);
        return -1;
    }
    _fseeki64(f,0,SEEK_END);
    size_t fileSize=_ftelli64(f);
    rewind(f);

    char *fileContent=malloc(fileSize+1);
    fread(fileContent,1,fileSize+1,f);
    fclose(f);



    printf("%p\n",proc);

    #ifdef NDEBUG
    int functionArgc=argc-7;
    if(oldBehavior)
    {
        functionArgc+=2;
    }
    #else
    int functionArgc=0;
    #endif

    INT_PTR procResult=0;


    if(functionArgc==0)
    {
        bool result=true;
        while(true)
        {
            printf("you're calling \"%s\" in \"%s\" without any arguments,is that what you meant to do ? [Y/n]:",processName,libraryName);
            char userInput=0;
            scanf("%c",&userInput);
            userInput&=~32;
            if(userInput=='N')
            {
                result=false;
                break;
            }
            if(userInput=='Y')
                break;
        }
        if(result)
        {
            procResult=proc();
            printf("result=%p\n",procResult);
        }
    }


    FreeLibrary(module);

}