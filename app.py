import serial
import time
import numpy as np
from keras.models import load_model
from PIL import Image, ImageOps
import cv2

# Load the model and set class names for organic and recyclable categories
model = load_model("keras_model.h5", compile=False)
class_names = ["Organic", "Recycle"]  # Ensure your model aligns with these two classes

try:
    # Set up serial communication with Arduino
    ser = serial.Serial('COM3', 9600, timeout=1)
    time.sleep(2)
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    ser = None

def classify_waste(img):
    np.set_printoptions(suppress=True)
    
    data = np.ndarray(shape=(1, 224, 224, 3), dtype=np.float32)
    image = img.convert("RGB")
    size = (224, 224)
    image = ImageOps.fit(image, size, Image.Resampling.LANCZOS)

    image_array = np.asarray(image)
    normalized_image_array = (image_array.astype(np.float32) / 127.5) - 1
    data[0] = normalized_image_array

    # Predict with the model
    prediction = model.predict(data)
    index = np.argmax(prediction)
    
    if index >= len(class_names):
        print("Warning: Model index out of bounds. Check model output alignment with class_names.")
        return "Unknown", 0.0
    
    class_name = class_names[index]
    confidence_score = prediction[0][index]

    return class_name.strip(), confidence_score

def send_command(command):
    if ser:
        ser.write(command.encode())
        print(f"Command '{command}' sent to Arduino")
    else:
        print("Serial port not available. Command not sent.")

# Real-time webcam classification
print("Starting webcam... Press 'q' to quit.")
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)  # Use DirectShow backend for faster initialization

try:
    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            break

        # Reduce frame size to speed up processing
        frame = cv2.resize(frame, (640, 480))

        # Convert the frame to a PIL image for classification
        frame_image = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
        label, confidence_score = classify_waste(frame_image)

        # Display the classification and confidence on the frame
        cv2.putText(frame, f"Class: {label}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
        cv2.putText(frame, f"Confidence: {confidence_score:.2f}", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2, cv2.LINE_AA)
        # Show the webcam feed
        cv2.imshow("Waste Classification", frame)
        # If high confidence (>0.8) for organic or recyclable waste, send appropriate command and pause for 30 seconds
        if confidence_score > 0.8:
            if label == "Recycle":
                print(f"Recycle waste detected with high confidence ({confidence_score:.2f}).")
                send_command('R')  # Command to move right
            elif label == "Organic":
                print(f"Organic waste detected with high confidence ({confidence_score:.2f}).")
                send_command('L')  # Command to move left
            
            # Wait for 30 seconds before resuming detection
            print("Pausing detection for 10 seconds...")
            time.sleep(10)


        # Exit the loop when 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

finally:
    cap.release()
    cv2.destroyAllWindows()
    if ser:
        ser.close()
    print("Serial connection closed.")
