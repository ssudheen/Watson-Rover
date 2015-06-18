cd /home/pi/Watson/VR
# Capture image
#raspistill -w 1555 -h 1166 -o vrImg.jpg
sleep 3
fswebcam -r 1555x1166 --no-banner vrImg.jpg
# Extract left and right images
python contours_sample_v0.7.py
# Remove borders
/home/pi/OCR/VR/VR_image_extraction_v0.3 left_p.jpg 60
/home/pi/OCR/VR/VR_image_extraction_v0.3 right_p.jpg 60
# compress output file
zip VR_watson.zip OP_left_p.jpg OP_right_p.jpg
# Process with watson
node VR_watson_analysis_v0.3.js

# Archive images - for demo purpose
dt=`date +%M%S`
mkdir ./archive/$dt
mv *.jpg ./archive/$dt/
