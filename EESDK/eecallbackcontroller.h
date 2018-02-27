#ifndef EEWRAPPER_H
#define EEWRAPPER_H


typedef enum {
    ErrorCallBack= -1,
    FileDownloaded = 1,
    FileUploaded = 2
} CallbackResult;

#ifdef __cplusplus
extern "C" {
#endif
    /**
     * @brief callBackFunctionFromC
     * Function, which controls callbacks from C.
     * After callback received, emit choosed method
     */
    void callBackFunctionFromC(CallbackResult result);

#ifdef __cplusplus
}
#endif
#endif // EEWRAPPER_H
