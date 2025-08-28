import numpy as np
import cv2 as cv

def gstreamer_pipeline(
    capture_width=1280,
    capture_height=720,
    display_width=1280,
    display_height=720,
    framerate=60,
    flip_method=0,
):
    return (
        "nvarguscamerasrc ! "
        "video/x-raw(memory:NVMM), "
        "width=(int)%d, height=(int)%d, "
        "format=(string)NV12, framerate=(fraction)%d/1 ! "
        "nvvidconv flip-method=%d ! "
        "video/x-raw, width=(int)%d, height=(int)%d, format=(string)BGRx ! "
        "videoconvert ! "
        "video/x-raw, format=(string)BGR ! appsink"
        % (
            capture_width,
            capture_height,
            framerate,
            flip_method,
            display_width,
            display_height,
        )
    )

print(gstreamer_pipeline(flip_method=6))
cap = cv.VideoCapture(gstreamer_pipeline(flip_method=6), cv.CAP_GSTREAMER)
if cap.isOpened():
	window_handle = cv.namedWindow("CSI Camera", cv.WINDOW_AUTOSIZE)
        # Window
	while cv.getWindowProperty("CSI Camera", 0) >= 0:
		ret_val, img = cap.read()
		#cv.imshow("CSI Camera", img)

	# Initiate FAST object with default values
		fast = cv.FastFeatureDetector_create()
		# find and draw the keypoints
		kp = fast.detect(img,None)
		img2 = cv.drawKeypoints(img, kp, None, color=(255,0,0))
		# Print all default params
		fast.setThreshold(30)	#Set threshold
		print( "Threshold: {}".format(fast.getThreshold()) )
		print( "nonmaxSuppression:{}".format(fast.getNonmaxSuppression()) )
		print( "neighborhood: {}".format(fast.getType()) )
		print( "Total Keypoints with nonmaxSuppression: {}".format(len(kp)) )
		#cv.imwrite('fast_true.png',img2)
		cv.imshow("CSI Camera", img2)
        	# This also acts as
		keyCode = cv.waitKey(30) & 0xFF
        	# Stop the program on the ESC key
		if keyCode == 27:
			break
	cap.release()
	cv.destroyAllWindows()
else:
	print("Unable to open camera")

