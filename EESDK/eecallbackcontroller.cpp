#include "eecallbackcontroller.h"
#include "eefileloader.h"
#include <QDebug>

extern EEFileLoader *gCryptoPointer;

/**
 * @brief callBackFunctionFromC
 * Function, which controls callbacks from C.
 * After callback received, emit choosed method
 */
void callBackFunctionFromC(CallbackResult result) {
    qDebug() << "Callback!";
    if (gCryptoPointer != nullptr) {
        gCryptoPointer->handleCallbackResult(result);
    }
}
