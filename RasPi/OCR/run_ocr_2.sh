cd ~/Watson/OCR
# take an image
fswebcam -r 640x480 --no-banner output.jpg
# crop and prepare for OCR
python prepare_OCR_v2.py
# run OCR
tesseract ./thres_img.jpg ./output2
# cleanup output
cat output2.txt | sed 's/\n/ /g' | sed 's/\_//g' | sed 's/[0-9]//' | > OP2.txt


# archive files - for demo purpose
dt=`date +%M%S`
mv output.jpg ./archive/output.$dt.jpg
mv crop_img.jpg ./archive/crop_img.$dt.jpg
mv thres_img.jpg ./archive/thres_img.$dt.jpg

