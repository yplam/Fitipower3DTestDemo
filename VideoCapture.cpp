#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <thread>
#include "webcam.h" // libwebcam

using namespace cv;
using namespace std;

#define CAM_ID 2
#define CAM_DEVICE "/dev/video2"

/**
 * Fitipower 18e3:5031 USB 双目摄像头测试代码
 * 依赖于 libwebcam https://sourceforge.net/projects/libwebcam/
 * 此摄像头默认只输出单目图像，参考 https://www.jianshu.com/p/2ce302d588a0
 * 进行配置
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {
  Mat origFrame, resizeFrame;
  VideoCapture cap;
  // 测试打开摄像头
  cap.open(CAM_ID);
  cout << "Open camera: " << CAM_ID << endl;
  cap.set(CAP_PROP_FRAME_WIDTH, 640);
  cap.set(CAP_PROP_FRAME_HEIGHT, 480);


  // 参考 https://www.jianshu.com/p/2ce302d588a0
  //  # simulate the SET_CUR sequence
  //  dev.ctrl_transfer(0x21,0x01,0x0800,0x0600,[0x50,0xff])
  //  dev.ctrl_transfer(0x21,0x01,0x0f00,0x0600,[0x00,0xf6])
  //  dev.ctrl_transfer(0x21,0x01,0x0800,0x0600,[0x25,0x00])
  //  dev.ctrl_transfer(0x21,0x01,0x0800,0x0600,[0x5f,0xfe])
  //  dev.ctrl_transfer(0x21,0x01,0x0f00,0x0600,[0x00,0x03])
  //  dev.ctrl_transfer(0x21,0x01,0x0f00,0x0600,[0x00,0x02])
  //  dev.ctrl_transfer(0x21,0x01,0x0f00,0x0600,[0x00,0x12])
  //  dev.ctrl_transfer(0x21,0x01,0x0f00,0x0600,[0x00,0x04])
  //  dev.ctrl_transfer(0x21,0x01,0x0800,0x0600,[0x76,0xc3])
  //  if kv in [1,2,3,4]:
  //  dev.ctrl_transfer(0x21,0x01,0x0a00,0x0600,[kv,0x00])

  CHandle handle = 0;
  CResult res = C_SUCCESS;
  res = c_init();
  if (res) {
    cout << "init error" << endl;
    return -1;
  }

  handle = c_open_device(CAM_DEVICE);
  if (!handle) {
    cout << "Unable to open device: " << CAM_DEVICE << endl;
    return -1;
  }
  uint16_t unit_id = 6;
  unsigned char selector;
  CControlValue value;
  value.type = CC_TYPE_RAW;
  value.raw.size = 2;
  value.raw.data = malloc(2);
  unsigned char entity[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00};
  uint8_t selectors[] = {0x08, 0x0f, 0x08, 0x08, 0x0f, 0x0f, 0x0f, 0x0f, 0x08, 0x0a};
  uint8_t datas[] = {0x50, 0xff, 0x00, 0xf6, 0x25, 0x00, 0x5f, 0xfe, 0x00, 0x03,
                     0x00, 0x02, 0x00, 0x12, 0x00, 0x04, 0x76, 0xc3, 0x04, 0x00};
  for (int step = 0; step < 10; step++) {
    selector = selectors[step];
    ((uint8_t *) value.raw.data)[0] = datas[step * 2];
    ((uint8_t *) value.raw.data)[1] = datas[step * 2 + 1];
    res = c_write_xu_control(handle, entity, unit_id, selector, &value);
    if (res) {
      cout << "Unable to set the control value" << endl;
      return -1;
    } else {
      cout << "set the control value 0x" << setfill('0')
           << setw(2) << hex << (int) datas[step * 2] << setfill('0')
           << setw(2) << (int) datas[step * 2 + 1] << endl;
    }
  }
  if (value.raw.data) free(value.raw.data);
  if (handle) c_close_device(handle);
  c_cleanup();
  Mat leftImage, rightImage;
  int imgCount = 1;
  while (true) {
    cap >> origFrame;

    resize(origFrame, resizeFrame, Size(640, 240), (0, 0), (0, 0), INTER_AREA);
    leftImage = resizeFrame(Rect(0, 0, 320, 240));
    rightImage = resizeFrame(Rect(320, 0, 320, 240));

    imshow("【左视图】", leftImage);
    imshow("【右视图】", rightImage);
    char key = waitKey(60);

    if (key == 27) {
      return 0;
    } else if (key == 's') {
      cout << "Save image" << endl;
      imwrite("left_" + std::to_string(imgCount) + ".jpg", leftImage);
      imwrite("right_" + std::to_string(imgCount) + ".jpg", rightImage);
      imgCount++;
    }
  }
  return 0;
}
