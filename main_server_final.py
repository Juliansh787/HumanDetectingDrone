## Web server가 Drone Client로 부터 로그 메세지 수신 확인

import threading
from socket import *
import cv2
import numpy as np
import time

faceCascade = cv2.CascadeClassifier('C:\opencv\sources\data\haarcascades\haarcascade_frontalface_default.xml')
logdata = 'empty'

# function to return received buffer from socket
def recvall(sock, count):
    # byte string
    buf = b''
    while count:
        newbuf = sock.recv(count)
        if not newbuf: return None
        buf += newbuf
        count -= len(newbuf)
    return buf

def recv_video_from_Drone(sock):     # get Drone cam image from Drone, and send image to KorenVM Web Server
    while True:
        # client에서 받은 stringData의 크기 (==(str(len(stringData))).encode().ljust(16))
        length = recvall(connectionSocket, 16)
        stringData = recvall(connectionSocket, int(length))
        data = np.fromstring(stringData, dtype='uint8')

        # data를 디코딩한다.
        frame = cv2.imdecode(data, cv2.IMREAD_COLOR)

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30),
            flags=cv2.CASCADE_SCALE_IMAGE
        )

        # Draw a rectangle around the faces
        for (x, y, w, h) in faces:
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        cv2.imshow('Drone Cam', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

def get_log_from_Drone(port):
    global logdata

    while logdata!="arrive":
        logdata = connectionSocket2.recv(1024)
        logdata = str(logdata).split("b'", 1)[1].rsplit("'", 1)[0]
        logdata = "{\"log\":\"" + logdata + "\"}"
        print(logdata)
        ## send receive message to drone
        connectionSocket2.sendall(str("Server message Get!").encode("utf-8"))
        ## send log to KorenVM Web server
    connectionSocket2.close()
    serverSocket2.close()
    print("Connect Finish")


if __name__=="__main__":

    print("Start Image processing Server")


    try:
        # Image Server
        ImgServer = "192.168.0.6"
        ImgServer_PORT = 22044
        serverSocket = socket(AF_INET, SOCK_STREAM)
        serverSocket.bind((ImgServer,ImgServer_PORT))
        serverSocket.listen(1)
        print("Waiting Drone Client")
        connectionSocket,addr = serverSocket.accept()
        print(str(addr),"에서 접속되었습니다.")

        try:
            # Log server
            ImgServer = "192.168.0.6"
            ImgServer_PORT2 = 22045
            serverSocket2 = socket(AF_INET, SOCK_STREAM)
            serverSocket2.bind((ImgServer, ImgServer_PORT2))
            serverSocket2.listen(1)
            print("Waiting Drone Client...")
            connectionSocket2, addr2 = serverSocket2.accept()
            print("Connect Drone!")

            receiver = threading.Thread(target=recv_video_from_Drone, args=(connectionSocket,))  # 영상 수신 및 전송 쓰레드
            log = threading.Thread(target=get_log_from_Drone, args=(connectionSocket2,))  # 로그 수신 쓰레드

            receiver.start()
            log.start()

            while True:
                time.sleep(1)  # thread 간의 우선순위 관계 없이 다른 thread에게 cpu를 넘겨줌
                pass  # sleep(0)은 cpu 선점권을 풀지 않음

        except Exception as e:
            print(e)
            connectionSocket2.close()  # 서버 닫기
            serverSocket2.close()
    except Exception as e:
        print(e)
        connectionSocket.close()    # 서버 닫기
        serverSocket.close()
