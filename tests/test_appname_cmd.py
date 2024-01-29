# import time
# from ledger_app_clients.ethereum.client import EthAppClient, StatusWord
# # from ledger_app_clients.boilerplate_response_unpacker import unpack_get_app_name_response
# import ledger_app_clients.ethereum.response_parser as ResponseParser
# import hashlib

# # In this test we check that the GET_APP_NAME replies the application name
# def test_app_name(backend):
#     # Use the app interface instead of raw interface
#     client = EthAppClient(backend)
#     # Send the GET_APP_NAME instruction to the app
#     with client.get_public_addr(display=False, chain_id=1):
#       pass
#     print("response: ", client.response().data)

#     _, addr, _ = ResponseParser.pk_addr(client.response().data)    

#     hex_string =''.join([f'{ byte:02X}' for byte in addr])
#     print(hex_string)
#     # m = hashlib.sha256()
#     print(hashlib.sha256(addr).hexdigest())

#     # print( addr.decode(), "hiii")
#     # u_string = codecs.decode(addr, 'ascii')

#     # print(u_string)
#     # Assert that we have received the correct appname
#     # assert unpack_get_app_name_response(response.data) == "expand"
