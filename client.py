# echo_client.py
# -*- coding:utf-8 -*-

import socket

# 접속 정보 설정
SERVER_IP = '192.168.0.2'
SERVER_PORT = 2204
SIZE = 512
SERVER_ADDR = (SERVER_IP, SERVER_PORT)


# 미리 지정되어 있는 드론존에 대한 좌표 밑 해당 target point의 번호
targetdata = [[0, 35.134833, 129.106817], [1, 35.134723, 129.105646], [2, 35.134277, 129.105293], [3, 35.134353, 129.105990],
              [4, 35.134572, 129.106477], [5, 35.134696, 129.105234], [6, 35.134387, 129.106875], [7, 35.134905, 129.106363]]

# 이 아래에 각 target point간의 distance와 angle에 대한 행렬 작성  이름 = movedata(?)

# 클라이언트 소켓 설정
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
    client_socket.connect(SERVER_ADDR)  # 서버에 접속
    msg = client_socket.recv(200)  # 서버로부터 응답받은 메시지 반환
    print("resp from server : {}".format(msg))  # 서버로부터 응답받은 메시지 출력

    while True:
        point = input('>>>')
        client_socket.send(point.encode('utf-8'))
        if point=='arrive':
            break;


# Server로부터 받은 각 좌표들을 위의 targetdata와 비교하고 행렬에서 다음 타겟으로 이동하기 위한 distance 값 밑 angle 추출

# 마브링크 등을 이용한 Pixhawk 통제
