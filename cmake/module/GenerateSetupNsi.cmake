# Copyright (c) 2023-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

function(generate_setup_nsi)
  set(abs_top_srcdir ${PROJECT_SOURCE_DIR})
  set(abs_top_builddir ${PROJECT_BINARY_DIR})
  set(CLIENT_URL ${PROJECT_HOMEPAGE_URL})
  set(CLIENT_TARNAME "groestlcoin")
  set(BITCOIN_WRAPPER_NAME "groestlcoin")
  set(BITCOIN_GUI_NAME "groestlcoin-qt")
  set(BITCOIN_DAEMON_NAME "groestlcoind")
  set(BITCOIN_CLI_NAME "groestlcoin-cli")
  set(BITCOIN_TX_NAME "groestlcoin-tx")
  set(BITCOIN_WALLET_TOOL_NAME "groestlcoin-wallet")
  set(BITCOIN_TEST_NAME "test_groestlcoin")
  set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})
  configure_file(${PROJECT_SOURCE_DIR}/share/setup.nsi.in ${PROJECT_BINARY_DIR}/groestlcoin-win64-setup.nsi USE_SOURCE_PERMISSIONS @ONLY)
endfunction()
