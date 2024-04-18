#Import necessary libraries
from flask import Flask, render_template, Response
import cv2
import time
import numpy as np
import threading


def current_milli_time():
    return round(time.time() * 1000)

#Initialize the Flask app
app = Flask(__name__)

camera = cv2.VideoCapture(0) 

camera_web = cv2.VideoCapture("http://pendelcam.kip.uni-heidelberg.de/mjpg/video.mjpg")

net = cv2.dnn.readNet('yolov3.weights', 'yolov3.cfg')
#net = cv2.dnn.readNet('yolov4-tiny.weights', 'yolov4-tiny.cfg')

net.setPreferableBackend(cv2.dnn.DNN_BACKEND_CUDA)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CUDA_FP16)
model = cv2.dnn_DetectionModel(net)
model.setInputParams(size=(416, 416), scale=1/255, swapRB=True)
 


'''
for ip camera use - rtsp://username:password@ip_address:554/user=username_password='password'_channel=channel_number_stream=0.sdp' 
for local webcam use cv2.VideoCapture(0)
''' 


last_frame = None
last_frame_time = 0

last_frame_web = None
last_frame_web_time = 0


sem = threading.Semaphore()
sem_frame = threading.Semaphore()
sem_frame_web = threading.Semaphore()


def get_frame_web(): 
    global last_frame_web , last_frame_web_time ,sem_frame_web    
    sem_frame_web.acquire()
    cur_time = current_milli_time()
    if (cur_time - last_frame_web_time > 10):  # 10ms is the maximum delay between frames 
        success, frame = camera_web.read()
        if   success:             
            last_frame_web = frame
            last_frame_web_time = cur_time
    if last_frame_web is None:
        #set frame to an empty image
        last_frame = np.zeros((480, 640, 3), np.uint8)        
    sem_frame_web.release()
    return last_frame_web

def get_frame():
    global last_frame , last_frame_time ,sem_frame    
    sem_frame.acquire()
    cur_time = current_milli_time()
    if (cur_time - last_frame_time > 10):  # 10ms is the maximum delay between frames 
        success, frame = camera.read()
        if   success:             
            last_frame = frame
            last_frame_time = cur_time
    if last_frame is None:
        #set frame to an empty image
        last_frame = np.zeros((480, 640, 3), np.uint8)        
    sem_frame.release()
    return last_frame
 
 
def getYoloImageResult(frame_in): 
    Conf_threshold = 0.4
    NMS_threshold = 0.4
    COLORS = [(0, 255, 0), (0, 0, 255), (255, 0, 0),
          (255, 255, 0), (255, 0, 255), (0, 255, 255)]
    frame = cv2.resize(frame_in, (416, 416))
    classes, scores, boxes = model.detect(frame, Conf_threshold, NMS_threshold)
    for (classid, score, box) in zip(classes, scores, boxes):
        color = COLORS[int(classid) % len(COLORS)]
        #label = "%s : %f" % (class_name[classid[0]], score)
        label = "..."
        cv2.rectangle(frame, box, color, 1)
        cv2.putText(frame, label, (box[0], box[1]-10),
                   cv2.FONT_HERSHEY_COMPLEX, 0.3, color, 1)
    #endingTime = time.time() - starting_time
    #fps = frame_counter/endingTime
    # print(fps)
    #cv.putText(frame, f'FPS: {fps}', (20, 50),  cv.FONT_HERSHEY_COMPLEX, 0.7, (0, 255, 0), 2)
    return frame


 

def gen_frames():  
    global net    
    global sem
    while True:
        start_frame_time = time.time()
        frame = get_frame()
        if (frame is None):
            break
        end_frame_time = time.time()
        frame_time = end_frame_time - start_frame_time
        frame_time = str(  int(frame_time * 1000)).encode()

        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n'+frame_time+ b'\r\n\r\n' + frame + b'\r\n')  # concat frame one by one and show result
        

def gen_frames_yolo():  
    global net    
    global sem
    while True:        
        frame = get_frame()
        if (frame is None):
            break 
 
        start_frame_time = time.time()
        sem.acquire()  #lock the  net usage 
        #resize to 417x417
        frame_scl = cv2.resize(frame, (417, 417))
        frame_out = getYoloImageResult(frame_scl)
        #resize back 
        frame_out = cv2.resize(frame_out, (frame.shape[1], frame.shape[0]))
        sem.release()
        end_frame_time = time.time()
        frame_time = end_frame_time - start_frame_time
        frame_time = str(  int(frame_time * 1000)).encode()

        ret, buffer = cv2.imencode('.jpg', frame_out)
        frame_out = buffer.tobytes()
        yield (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n'+frame_time+ b'\r\n\r\n' + frame_out + b'\r\n')  # concat frame one by one and show result

#check if last_frame is None
def gen_frames_BW():     
    while True: 
    
        start_frame_time = time.time()
        frame = get_frame()
        if (frame is None):
            break
    
        #process frame 
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        #normalize the image
        gray = cv2.normalize(gray, None, 0, 255, cv2.NORM_MINMAX)
        #increase contrast
        gray = cv2.equalizeHist(gray)     

        end_frame_time = time.time()
        frame_time = end_frame_time - start_frame_time
        frame_time = str(  int(frame_time * 1000)).encode()


        ret, buffer = cv2.imencode('.jpg', gray)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n'+frame_time+ b'\r\n\r\n' + frame + b'\r\n')   # concat frame one by one and show result
#check if last_frame is None


def gen_frames_web():     
    while True: 
    
        start_frame_time = time.time()
        frame = get_frame_web()
        if (frame is None):
            break 

        end_frame_time = time.time()
        frame_time = end_frame_time - start_frame_time
        frame_time = str(  int(frame_time * 1000)).encode()


        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n'+frame_time+ b'\r\n\r\n' + frame + b'\r\n')   # concat frame one by one and show result




def gen_sensor():
    import random
    while True:         
        #get sensor data for power usage in this PC 
        power_usage =  ( int(time.time() ) % 10  ) + random.randint(0, 4)  
        power_usage = str.encode(str( power_usage) )
        return  ( power_usage +  b' \r\n' )

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/video_bw')
def video_bw():
    return Response(gen_frames_BW(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/video_yolo')
def video_yolo():
    return Response(gen_frames_yolo(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/video_web')
def video_web():
    return Response(gen_frames_web(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/sensor')
def sensor():
    ## return text data
    return Response(gen_sensor(), mimetype='text/plain')



if __name__ == "__main__":
    last_frame = None 
    app.run(debug=True)