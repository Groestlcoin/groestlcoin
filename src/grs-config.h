#pragma once

#define UCFG_DEFINE_NDEBUG 0

//  Check if this can be removed
#define BUILD_DESC "v26.0.0"

#if defined(_MSC_VER) && !defined(RC_COMPILER)
#	include <vc-inc.h>
#endif

#pragma warning(disable: 4018 4101 4146 4242 4244 4267 4290 4334 4717 4789 4800 4804)

#define WIN32

#define _CRT_SECURE_NO_DEPRECATE 1
#define _SCL_SECURE_NO_WARNINGS

#define HAVE_DECL_HTOBE16 1
#define HAVE_DECL_HTOLE16 1
#define HAVE_DECL_BE16TOH 1
#define HAVE_DECL_LE16TOH 1
#define HAVE_DECL_HTOLE32 1
#define HAVE_DECL_BE32TOH 1
#define HAVE_DECL_LE32TOH 1
#define HAVE_DECL_HTOBE64 1
#define HAVE_DECL_HTOLE64 1
#define HAVE_DECL_BE64TOH 1
#define HAVE_DECL_LE64TOH 1

#define HAVE_DECL_STRNLEN 1
#define HAVE_DECL_EVP_MD_CTX_NEW 1

#define HAVE_WORKING_BOOST_SLEEP_FOR 1

#define BOOST_ASIO_ERROR_CATEGORY_NOEXCEPT noexcept
