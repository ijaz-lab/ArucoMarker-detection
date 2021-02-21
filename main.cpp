#include <iostream>

#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

int main(int argc, char** argv)
{
  // Check cv versionmake
  std::cout << "Using OpenCV version: " << CV_VERSION << std::endl;

  // Create video input
  cv::VideoCapture inputVideo;
  int input_source(0);
  if (argc > 1)
    input_source = std::atoi(argv[1]);
  if (!inputVideo.open(input_source))
  {
    std::cerr << "Error opening input video soruce: " << input_source << std::endl;
    return EXIT_FAILURE;
  }

  // Create marker dictionary
  auto marker_dict = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME::DICT_4X4_50);

  // Get imshow ready
  cv::namedWindow("Display window", cv::WINDOW_KEEPRATIO | cv::WINDOW_NORMAL);
  cv::resizeWindow("Display window", 800, 600);

  // Grab images until escape is pressed
  int key = 0;
  while (key != 27 && inputVideo.grab())
  {
    // Retrieve image
    cv::Mat image;
    inputVideo.retrieve(image);

    // Get image output ready
    cv::Size image_size = image.size();
    cv::Mat out_image(image_size.height, 3 * image_size.width, CV_8UC3);
    cv::Mat left(out_image, cv::Rect(0, 0, image_size.width, image_size.height));
    image.copyTo(left);
    cv::Mat mid(out_image, cv::Rect(image_size.width, 0, image_size.width, image_size.height));
    image.copyTo(mid);
    cv::Mat right(out_image, cv::Rect(2 * image_size.width, 0, image_size.width, image_size.height));
    image.copyTo(right);

    // Add names to images
    int corner_offset = 50;
    cv::putText(left, "Original image", cv::Point(corner_offset, corner_offset), cv::FONT_HERSHEY_DUPLEX, 1.0,
                CV_RGB(0, 0, 0), 2);
    cv::putText(mid, "Image with OpenCV drawing", cv::Point(corner_offset, corner_offset), cv::FONT_HERSHEY_DUPLEX, 1.0,
                CV_RGB(0, 0, 0), 2);
    cv::putText(right, "Image with custom drawing", cv::Point(corner_offset, corner_offset), cv::FONT_HERSHEY_DUPLEX,
                1.0, CV_RGB(0, 0, 0), 2);

    // Detect markers
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f> > corners;
    cv::aruco::detectMarkers(image, marker_dict, corners, ids);

    // Draw markers using opencv tool
    cv::aruco::drawDetectedMarkers(mid, corners, ids);

    // Draw markers custom
    for (size_t i = 0; i < corners.size(); ++i)
    {
      // Convert to integer ponits
      int num = static_cast<int>(corners[i].size());
      std::vector<cv::Point> points;
      for (size_t j = 0; j < corners[i].size(); ++j)
        points.push_back(cv::Point(static_cast<int>(corners[i][j].x), static_cast<int>(corners[i][j].y)));
      const cv::Point* pts = &(points[0]);

      // Draw
      cv::fillPoly(right, &pts, &num, 1, cv::Scalar(255, 0, 0));

      // Draw contour
      for (size_t j = 0; j < corners[i].size(); ++j)
      {
        size_t next = (j + 1) % corners[i].size();
        cv::line(right, corners[i][j], corners[i][next], cv::Scalar(0, 255, 0), 5);
      }
    }

    // Display
    cv::imshow("Display window", out_image);
    key = cv::waitKey(5);
  }

  return EXIT_SUCCESS;
}
