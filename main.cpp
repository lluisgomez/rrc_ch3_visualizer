#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>
#include <tinyxml.h>

using namespace cv;
using namespace std;


int main (int argc, char* argv[] )
{
    if (argc < 3)
    {
      cerr << "  visualizer <XML_GT_FILE> <MP4_VIDEO_FILE>" << endl;
      return 1;
    }

    TiXmlDocument doc( argv[1] );
    if (!doc.LoadFile()) {
        cerr << "Failed to open XML file!\n" << endl;
        cerr << "  visualizer <XML_GT_FILE> <MP4_VIDEO_FILE>" << endl;
        return 1;
    }
    TiXmlElement* xml_frame = doc.FirstChild( "Frames" )->FirstChild( "frame" )->ToElement();


    std::string arg = argv[2];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file
    if (!capture.isOpened()) {
        cerr << "Failed to open video file!\n" << endl;
        cerr << "  visualizer <XML_GT_FILE> <MP4_VIDEO_FILE>" << endl;
        return 1;
    }

    string window_name(argv[2]);
    cout << "press space to pause. q or esc to quit" << endl;
    namedWindow(window_name, CV_WINDOW_KEEPRATIO|WINDOW_AUTOSIZE ); //resizable window;
    Mat frame;
    int n = 1;
    bool paused = false;
    for (;;) {
        if (!paused)
          capture >> frame;

        if (frame.empty())
            break;
        fprintf(stdout,"frame %d ID %d\n", n, atoi(xml_frame->Attribute( "ID")));
        
        TiXmlNode* xml_object_node = xml_frame->FirstChild("object");
        if (xml_object_node)
        {
            TiXmlElement* xml_object = xml_object_node->ToElement();
            for( xml_object; xml_object; xml_object=xml_object->NextSiblingElement() )
    	    {
                
                string id = xml_object->Attribute( "ID");
                string transcription = xml_object->Attribute( "Transcription");
                string quality = xml_object->Attribute( "Quality");
                Scalar color = Scalar(255,0,0);
                if (quality==string("MODERATE"))
                   color = Scalar(0,255,255);
                if (transcription == string("#Don't Care#")|| quality==string("LOW"))
                   color = Scalar(0,0,255);

                TiXmlElement* xml_point = xml_object->FirstChild("Point")->ToElement();
                vector<Point> ps;
                for( xml_point; xml_point; xml_point=xml_point->NextSiblingElement() )
                {
                   ps.push_back(Point(atoi(xml_point->Attribute( "x")), 
                                      atoi(xml_point->Attribute( "y"))));
                }
                Rect box = boundingRect(ps);
                rectangle(frame,box,color,1,8,0);
                if (quality != string("LOW"))
                   putText(frame,transcription,box.tl(),CV_FONT_HERSHEY_PLAIN,1,color,2);
            }
        }
        imshow(window_name, frame);
        char key = (char)waitKey(45); //delay N millis, usually long enough to display and capture input
        switch (key) {
          case 'q':
          case 'Q':
          case 27: //escape key
            return 0;
          case ' ': //Pause
            paused = !paused;
            break;
          default:
            break;
        }

        if (!paused)
        {
          xml_frame = xml_frame->NextSiblingElement();
          n++;
        }
    }
    return 0;
}
