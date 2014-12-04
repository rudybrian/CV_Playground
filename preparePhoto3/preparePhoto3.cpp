/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/face.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/core/ocl.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include <stdio.h>
#include <dirent.h>

#define TRACE 1		// for trace fonction

using namespace cv;
using namespace cv::face;
using namespace std;

Point2i Myeye_left;
Point2i Myeye_right;

///////////////////////////////////////////////////
// trace fonction, output only if #define TRACE=1
///////////////////////////////////////////////////
void trace(string s)
{
	if (TRACE==1)
	{
		cout<<s<<"\n";
	}
}

//////////////////////////////////////////////
// compute distance btw 2 points
//////////////////////////////////////////////
float Distance(Point2i p1, Point2i p2)
{
	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	return sqrt(dx*dx+dy*dy);
}


int main(int argc, const char *argv[]) {
    // Check for valid command line arguments, print usage
    // if no arguments were given.
    if (argc != 1) {
        cout << "usage: " << argv[0] << endl;
        exit(1);
    }
    string fn_haar_face = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
    string fn_haar_eyes = "/usr/local/share/OpenCV/haarcascades/haarcascade_eye.xml";
    //
    CascadeClassifier haar_cascade_face;
    haar_cascade_face.load(fn_haar_face);
    CascadeClassifier haar_cascade_eyes;
    haar_cascade_eyes.load(fn_haar_eyes);

    // quality type JPG to save image
    std::vector<int> qualityType;
    qualityType.push_back(1);
    qualityType.push_back(90);

    // Holds the current frame from the Video device:
    Mat frame;
    // read the current directory
    DIR * rep =opendir(".");
    if (rep==NULL) return 0;
    
    struct dirent *ent;
    int index=1;
    while((ent=readdir(rep)) != NULL) {
	int nLen = strlen(ent->d_name);
	char * imageName = ent->d_name;

	// read extention, only keep jpg file
	if ((nLen>4) && (imageName[nLen-1]=='g')&&(imageName[nLen-2]=='p')&&(imageName[nLen-3]=='j')) {

		trace("Reading : "+string(imageName));
		frame = imread(imageName,1);

		if(!frame.empty()) {
        		// Clone the current frame:
        		Mat original = frame.clone();
			Mat resized;
			// resize picture
			if (original.size().width > 1024) {
				trace("- image needs to be resized");
				// Resize src to img size 
				Size oldTaille = original.size(); 
				Size newTaille(1024,oldTaille.height*1024/oldTaille.width);
				cv::resize(original, resized, newTaille); 
				trace("- done resizing");
			} else {
				resized = original.clone();
			}


        		// Convert the current frame to grayscale:
        		Mat gray;
			trace("- creating gray image");
        		cvtColor(resized, gray, COLOR_BGR2GRAY);

			// equalize the face image 
			Mat grayeq;
			trace("- equalizing gray image");
			equalizeHist(gray, grayeq);

        		// Find the faces in the frame:
        		vector< Rect_<int> > faces;
			trace("- searching for faces");
        		//haar_cascade_face.detectMultiScale(grayeq, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(50, 50));
        		//haar_cascade_face.detectMultiScale(grayeq, faces);
        		//haar_cascade_face.detectMultiScale(gray, faces);
        		haar_cascade_face.detectMultiScale(grayeq, faces);
			if (faces.size()==0) {
				trace("- no face found. Moving on...");
			}
			else {
				// At this point you have the position of the faces in
				// faces. Now we'll get the facesi
				// Process face by face:
				Rect face_i = faces[0];
				// Crop the face from the image. So simple with OpenCV C++:
				//Mat face = grayeq(face_i);
				Mat face = gray(face_i);
				// Find the eyes (no glasses)
				vector< Rect_<int> > eyes;
				trace("- found a face, now searching for eyes");
				haar_cascade_eyes.detectMultiScale(face, eyes, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30));
				size_t noeyes = 0;
				if (eyes.size()==2) {
					trace("-- face without glasses");
					for (size_t j = 0; j < 2; j++) {
						Point eye_center( faces[0].x + eyes[1-j].x + eyes[1-j].width/2, faces[0].y + eyes[1-j].y + eyes[1-j].height/2 );	
						if (j==0) { // left eye
							Myeye_left.x =eye_center.x;
							Myeye_left.y =eye_center.y;
						}
						if (j==1) { // right eye
							Myeye_right.x =eye_center.x;
							Myeye_right.y =eye_center.y;
						}
					}
				} else {
					noeyes = 1;
					trace("- no valid eyes found in face image. Moving on...");
				}
			
				// Sometimes the eyes are reversed, so we need to flip them
				if (Myeye_right.x < Myeye_left.x) {
					int tmpX = Myeye_right.x;
					int tmpY = Myeye_right.y;
					Myeye_right.x=Myeye_left.x;
					Myeye_right.y=Myeye_left.y;
					Myeye_left.x=tmpX;
					Myeye_left.y=tmpY;
					trace("-- eyes reversed, flipping");
				}
	
				if (noeyes == 0) {
					trace ("- start cropFace");
					
					Point2i Myoffset_pct;
					Myoffset_pct.x =100.0 * 0.3;
					Myoffset_pct.y = Myoffset_pct.x;

					// size of new picture
					Point2i Mydest_sz;
					Mydest_sz.x = 100;	
					Mydest_sz.y = Mydest_sz.x;

					// calculate offsets in original image
					int offset_h = (Myoffset_pct.x*Mydest_sz.x/100);
					int offset_v = (Myoffset_pct.y*Mydest_sz.y/100);

					// get the direction
					Point2i eye_direction;
					eye_direction.x = Myeye_right.x - Myeye_left.x;
					eye_direction.y = Myeye_right.y - Myeye_left.y;

					// calc rotation angle in radians
					float rotation = atan2((float)(eye_direction.y),(float)(eye_direction.x));
	
					// distance between them
					float dist = Distance(Myeye_left, Myeye_right);

					// calculate the reference eye-width
					int reference = Mydest_sz.x - 2*offset_h;

					// scale factor
					float scale = dist/(float)reference;

					// rotate original around the left eye
					char sTmp[16];
					sprintf(sTmp,"%f",rotation);
					trace("-- rotate image "+string(sTmp));
					Point2f src_center(Myeye_left.x, Myeye_left.y);
					// conversion in degrees
					double angle = rotation*180.0/3.14157;
					Mat rot_matrix = getRotationMatrix2D(src_center, angle, 1.0);
					Mat rotated_img(Size(resized.size().height, resized.size().width), resized.type());
					warpAffine(resized, rotated_img, rot_matrix, resized.size());

					// crop the rotated image
					Point2i crop_xy;
					crop_xy.x = Myeye_left.x - scale*offset_h;
					crop_xy.y = Myeye_left.y - scale*offset_v;
	
					Point2i crop_size;
					crop_size.x = Mydest_sz.x*scale; 
					crop_size.y = Mydest_sz.y*scale;
				
					trace("-- crop image");
					cv::Rect myROI(crop_xy.x, crop_xy.y, crop_size.x, crop_size.y);
					if ((crop_xy.x+crop_size.x<original.size().width)&&(crop_xy.y+crop_size.y<original.size().height)) {
						rotated_img = rotated_img(myROI);
						//resize it
						trace("-- resize image");
						Mat face_resized;
						cv::resize(rotated_img, face_resized, Size(Mydest_sz), 1.0, 1.0, INTER_CUBIC);
						// write the resulting face image to a file
						char newName[16];
						sprintf(newName,"%s%d.jpg","p",index);
						string newNameS(newName);
						// convert to grayscale
						Mat grayframe;
						trace("- transform : gray");
						cvtColor(face_resized, grayframe, COLOR_BGR2GRAY);
						// equalize histo color
						trace("- transforme : equalize histo");
						Mat eqgrayframe;
						equalizeHist(grayframe, eqgrayframe);
						trace("- save image "+newNameS);
						imwrite(newNameS,eqgrayframe,qualityType);
						//imwrite(newNameS,grayframe,qualityType);
					}
					else {
						trace("-- error cropping");
					}
				}
			}
		} else {
			trace("- image is empty, moving on...");
		}
		index++;
	}
    }
    return 0;
}
