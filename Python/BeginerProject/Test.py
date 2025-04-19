import cv2
import tensorflow as tf
from tensorflow.keras.applications import MobileNetV2
from tensorflow.keras.preprocessing.image import img_to_array
from tensorflow.keras.applications.mobilenet_v2 import preprocess_input
import numpy as np

# Load pre-trained face and eye detectors from OpenCV
face_detector = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')
eye_detector = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_eye.xml')

# Load the pre-trained model for face detection
model = MobileNetV2(weights="imagenet", include_top=False)

def detect_faces(frame):
    # Convert frame to grayscale (required for OpenCV face detector)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_detector.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30), flags=cv2.CASCADE_SCALE_IMAGE)
    return faces

def detect_eyes(face):
    gray_face = cv2.cvtColor(face, cv2.COLOR_BGR2GRAY)
    eyes = eye_detector.detectMultiScale(gray_face, scaleFactor=1.1, minNeighbors=10, minSize=(20, 20))
    return eyes

def preprocess_face(face_image):
    face_image = cv2.resize(face_image, (224, 224))
    face_image = img_to_array(face_image)
    face_image = preprocess_input(face_image)
    face_image = np.expand_dims(face_image, axis=0)
    return face_image

# Open video capture (0 for default camera)
cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    faces = detect_faces(frame)
    for (x, y, w, h) in faces:
        face = frame[y:y + h, x:x + w]
        preprocessed_face = preprocess_face(face)
        predictions = model.predict(preprocessed_face)

        # Draw rectangle around the face
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        # Detect and draw rectangles around eyes within the detected face
        eyes = detect_eyes(face)
        for (ex, ey, ew, eh) in eyes:
            cv2.rectangle(face, (ex, ey), (ex + ew, ey + eh), (255, 0, 0), 2)

    cv2.imshow('Face and Eye Detection', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
