#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    //Done: Request a service and pass the velocities to it to drive the robot

    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
        ROS_ERROR("Failed to call the service drive_bot");
}

// This callback function continuously executes and reads the image data

void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;	//White ball pixel declaration
    bool ball_image = false;
    int ball_pixel = 0;		// ball position


    // Done: Loop through each pixel in the (Image img) and check if there's a bright white one
  
    for (int i = 0; i < img.height * img.step; i+=3)
 	{
      if ((img.data[i] == white_pixel) && (img.data[i+1] == white_pixel) && (img.data[i+2] == white_pixel)) 
	{
          ball_image = true;
          ball_pixel = i % img.step;
          break;
     	 }
 	}
  
 // Done: Identify if this pixel falls in the left, mid, or right side of the image		and:
// Done:Depending on the white ball position, call the drive_bot function and pass velocities to it
 
    if(ball_image == true)
	{
	ROS_INFO_STREAM("White ball seen;Driving towards it");

      if(ball_pixel < (img.step/3))
	{
	
        drive_robot(0.1,0.5);	//Drive left
    	}
      else if(ball_pixel < (2*img.step/3))
	{
	  
        drive_robot(0.5,0.0);	//Drive straight
        }
      else
	{
	
        drive_robot(0.1,-0.5);	//Drive Right
        }
         }
	// Request a stop when there's no white ball seen by the camera
    else{
	drive_robot(0.0,0.0);
      ROS_INFO_STREAM("No White ball seen");
      
   	 }
	}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
