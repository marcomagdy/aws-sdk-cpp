include(CheckCXXSourceCompiles)
# Zlib
if(PLATFORM_ANDROID AND ANDROID_BUILD_ZLIB)
    set(BUILD_ZLIB 1)
    message(STATUS "  Building Zlib as part of AWS SDK")
elseif(NOT PLATFORM_WINDOWS AND NOT PLATFORM_CUSTOM)
    include(FindZLIB)
    if(NOT ZLIB_FOUND)
        message(FATAL_ERROR "Could not find zlib")
    else()
        message(STATUS "  Zlib include directory: ${ZLIB_INCLUDE_DIRS}")
        message(STATUS "  Zlib library: ${ZLIB_LIBRARIES}")
    endif()
    List(APPEND EXTERNAL_DEPS_INCLUDE_DIRS ${ZLIB_INCLUDE_DIRS})
endif()

# Encryption control
if(NOT NO_ENCRYPTION)
    if(PLATFORM_WINDOWS)
        set(ENABLE_BCRYPT_ENCRYPTION ON)
    elseif(PLATFORM_LINUX OR PLATFORM_ANDROID)
        set(ENABLE_OPENSSL_ENCRYPTION ON)
    elseif(PLATFORM_APPLE)
        set(ENABLE_COMMONCRYPTO_ENCRYPTION ON)
    endif()
else()
    set(ENABLE_INJECTED_ENCRYPTION ON)
endif()

if(ENABLE_BCRYPT_ENCRYPTION)
    add_definitions(-DENABLE_BCRYPT_ENCRYPTION)
    set(CRYPTO_LIBS Bcrypt)
    set(CRYPTO_LIBS_ABSTRACT_NAME Bcrypt)
    message(STATUS "Encryption: Bcrypt")
elseif(ENABLE_OPENSSL_ENCRYPTION)
    add_definitions(-DENABLE_OPENSSL_ENCRYPTION)
    message(STATUS "Encryption: Openssl")

    if(PLATFORM_ANDROID AND ANDROID_BUILD_OPENSSL)
        set(BUILD_OPENSSL 1)
        message(STATUS "  Building Openssl as part of AWS SDK")
    else()
        include(FindOpenSSL)
        if(NOT OPENSSL_FOUND)
            message(FATAL_ERROR "Could not find openssl")
        else()
            message(STATUS "  Openssl include directory: ${OPENSSL_INCLUDE_DIR}")
            message(STATUS "  Openssl library: ${OPENSSL_LIBRARIES}")
        endif()
        List(APPEND EXTERNAL_DEPS_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
    endif()
    set(CRYPTO_LIBS ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES})
    # ssl depends on libcrypto
    set(CRYPTO_LIBS_ABSTRACT_NAME crypto ssl z)
elseif(ENABLE_COMMONCRYPTO_ENCRYPTION)
    add_definitions(-DENABLE_COMMONCRYPTO_ENCRYPTION)
    message(STATUS "Encryption: CommonCrypto")
elseif(ENABLE_INJECTED_ENCRYPTION)
    message(STATUS "Encryption: None")
    message(STATUS "You will need to inject an encryption implementation before making any http requests!")
endif()

# Http client control
if(NOT NO_HTTP_CLIENT)
    if(PLATFORM_WINDOWS)
        if(FORCE_CURL)
            set(ENABLE_CURL_CLIENT 1)
        else()
            set(ENABLE_WINDOWS_CLIENT 1)
        endif()
    elseif(PLATFORM_LINUX OR PLATFORM_APPLE OR PLATFORM_ANDROID)
        set(ENABLE_CURL_CLIENT 1)
    endif()

    if(ENABLE_CURL_CLIENT)
        add_definitions(-DENABLE_CURL_CLIENT)
        message(STATUS "Http client: Curl")

        if(PLATFORM_ANDROID AND ANDROID_BUILD_CURL)
            set(BUILD_CURL 1)
            message(STATUS "  Building Curl as part of AWS SDK")
        else()
            include(FindCURL)
            if(NOT CURL_FOUND)
                message(FATAL_ERROR "Could not find curl")
            else()
                message(STATUS "  Curl include directory: ${CURL_INCLUDE_DIRS}")
                message(STATUS "  Curl library: ${CURL_LIBRARIES}")
            endif()
            
            # HTTP2 was added to curl from 7.33. But a better flag CURL_HTTP_VERSION_2TLS requires curl 7.47.0 and is set to default after curl 7.62.0
            # This flag was added as an enum member in curl.h and can't be found if we perform check_cxx_symbol_exists
            set(HTTP2_TEST_SOURCE "
                #include <curl/curl.h>
                int main() {
                    int x = CURL_HTTP_VERSION_2TLS;
                    return x;
                }")
            unset(CURL_HAS_HTTP2 CACHE)
            check_cxx_source_compiles("${HTTP2_TEST_SOURCE}" CURL_HAS_HTTP2)
            if (CURL_HAS_HTTP2)
                add_definitions(-DCURL_HTTP2_SUPPORTED)
            endif()

            List(APPEND EXTERNAL_DEPS_INCLUDE_DIRS ${CURL_INCLUDE_DIRS})
        endif()

        if(TEST_CERT_PATH)
            message(STATUS "Setting curl cert path to ${TEST_CERT_PATH}")
            add_definitions(-DTEST_CERT_PATH="\"${TEST_CERT_PATH}\"")
        endif()

        set(CLIENT_LIBS ${CURL_LIBRARIES})
        set(CLIENT_LIBS_ABSTRACT_NAME curl)
    elseif(ENABLE_WINDOWS_CLIENT)
        add_definitions(-DENABLE_WINDOWS_CLIENT)

        if(USE_IXML_HTTP_REQUEST_2)
            add_definitions(-DENABLE_WINDOWS_IXML_HTTP_REQUEST_2_CLIENT)
            set(CLIENT_LIBS msxml6 runtimeobject)
            set(CLIENT_LIBS_ABSTRACT_NAME msxml6 runtimeobject)
            message(STATUS "Http client: IXmlHttpRequest2")
            if(BYPASS_DEFAULT_PROXY)
                add_definitions(-DBYPASS_DEFAULT_PROXY)
                list(APPEND CLIENT_LIBS winhttp)
                list(APPEND CLIENT_LIBS_ABSTRACT_NAME winhttp)
                message(STATUS "Proxy bypass is enabled via WinHttp")
            endif()
        else()
            set(CLIENT_LIBS Wininet winhttp)
            set(CLIENT_LIBS_ABSTRACT_NAME Wininet winhttp)
            message(STATUS "Http client: WinHttp")
        endif()

        if ("winhttp" IN_LIST CLIENT_LIBS)
            # Start from Windows 10, version 1507, WinINet supports HTTP2.
            # https://docs.microsoft.com/en-us/windows/desktop/WinInet/option-flags#INTERNET_OPTION_ENABLE_HTTP_PROTOCOL
            # 1507 related latest build number is 10240 based on https://en.wikipedia.org/wiki/Windows_10_version_history
            # Start from Windows 10, version 1607, WinHttp supports HTTP2.
            # https://docs.microsoft.com/en-us/windows/desktop/WinHttp/option-flags#WINHTTP_OPTION_ENABLE_HTTP_PROTOCOLa
            # 1607 related latest build number is 14393 based on https://en.wikipedia.org/wiki/Windows_10_version_history
            message(STATUS "Windows SDK Version: ${CMAKE_SYSTEM_VERSION}")

            unset (WINHTTP_HAS_HTTP2 CACHE)
            unset (WININET_HAS_HTTP2 CACHE)

            set(HTTP2_TEST_SOURCE "
                #include <Windows.h>
                #include <winhttp.h>
                int main() {
                    int x = WINHTTP_PROTOCOL_FLAG_HTTP2;
                    return x;
                }")
            check_cxx_source_compiles("${HTTP2_TEST_SOURCE}" WINHTTP_HAS_HTTP2)

            set(HTTP2_TEST_SOURCE "
                #include <Windows.h>
                #include <WinInet.h>
                int main() {
                    int x = HTTP_PROTOCOL_FLAG_HTTP2;
                    return x;
                }")
            check_cxx_source_compiles("${HTTP2_TEST_SOURCE}" WININET_HAS_HTTP2)
        
            if (WININET_HAS_HTTP2)
                add_definitions(-DWININET_HTTP2_SUPPORTED)
            endif()

            if (WINHTTP_HAS_HTTP2)
                add_definitions(-DWINHTTP_HTTP2_SUPPORTED)
            endif()
        endif()
    else()
        message(FATAL_ERROR "No http client available for target platform and client injection not enabled (-DNO_HTTP_CLIENT=ON)")
    endif()
else()
    message(STATUS "You will need to inject an http client implementation before making any http requests!")
endif()

if (EXTERNAL_DEPS_INCLUDE_DIRS)
    List(REMOVE_DUPLICATES EXTERNAL_DEPS_INCLUDE_DIRS)
endif()
