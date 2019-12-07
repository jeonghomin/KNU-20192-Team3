#define S_OK ((HRESULT)0L)
//Standard Library
#include <iostream>
#include <stdio.h>
#include<fstream>
#include<string>
//OpenCV Header
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<opencv2\video\background_segm.hpp>
#include <Kinect.h>
using namespace cv;
using namespace std;
// Kinect for Windows SDK Header


const   string  get_name(int n);   
int main(void)
{
	string filePath = "non_person.txt";
	ofstream writeFile(filePath.data());

	IKinectSensor* mySensor = nullptr;
	GetDefaultKinectSensor(&mySensor);
	mySensor->Open();

	int myBodyCount = 0;
	IBodyFrameSource* myBodySource = nullptr;
	IBodyFrameReader* myBodyReader = nullptr;
	mySensor->get_BodyFrameSource(&myBodySource);
	myBodySource->OpenReader(&myBodyReader);
	myBodySource->get_BodyCount(&myBodyCount);


	IDepthFrameSource* myDepthSource = nullptr;
	IDepthFrameReader* myDepthReader = nullptr;
	mySensor->get_DepthFrameSource(&myDepthSource);
	myDepthSource->OpenReader(&myDepthReader);


	int height = 0, width = 0;
	IFrameDescription* myDescription = nullptr;;
	myDepthSource->get_FrameDescription(&myDescription);
	myDescription->get_Height(&height);
	myDescription->get_Width(&width);   //위는 깊이 데이터와 뼈 데이터를 준비하기위한 것입니다.Reader

	IBodyFrame* myBodyFrame = nullptr;
	IDepthFrame* myDepthFrame = nullptr;
	Mat img16(height, width, CV_16UC1); //깊이 이미지 표시 준비
	Mat img8(height, width, CV_8UC1);

	while (1)
	{
		while (myDepthReader->AcquireLatestFrame(&myDepthFrame) != S_OK);
		myDepthFrame->CopyFrameDataToArray(width * height, (UINT16*)img16.data);
		img16.convertTo(img8, CV_8UC1, 255.0 / 4500);
		imshow("Depth Img", img8);  //심도 이미지 변환 및 디스플레이

		while (myBodyReader->AcquireLatestFrame(&myBodyFrame) != S_OK);

		int myBodyCount = 0;
		IBody * *bodyArr = nullptr;
		myBodySource->get_BodyCount(&myBodyCount);//카메라로 촬영 한 총 인체 수
		bodyArr = new IBody * [myBodyCount];
		for (int i = 0; i < myBodyCount; i++)   //bodyArr초기화
			bodyArr[i] = nullptr;

		myBodyFrame->GetAndRefreshBodyData(myBodyCount, bodyArr);

		for (int i = 0; i < myBodyCount; i++)   //6 명 순회
		{
			BOOLEAN     result = false;
			if (bodyArr[i]->get_IsTracked(&result) == S_OK && result)   //이 사람이 감지되었는지 확인
			{
				cout << "Body " << i << " tracked!" << endl;

				int count = 0;
				Joint   jointArr[JointType_Count];
				bodyArr[i]->GetJoints(JointType_Count, jointArr);    //이 사람에 대한 공동 데이터를 가져옴

				if (writeFile.is_open()) {

					for (int j = 0; j < JointType_Count; j++)
					{
						if (jointArr[j].TrackingState != TrackingState_Tracked) //감지 된 관절이 표시되는지 확인함
							continue;
						string  rt = get_name(jointArr[j].JointType);   //관절이름 얻기
						if (rt != "NULL")   //조인트 정보 출력

						{
							count++;
							cout << "   " << rt << " tracked" << endl;
							if (rt == "Head") {
								cout << "       Head: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
								writeFile << "Head:" << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
							}
							if (rt == "Neck") {
								cout << "       Neck: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
								writeFile << "Neck:" << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
							}
							if (rt == "Spine_Shoulder") {
								cout << "       Spine_Shoulder: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
								writeFile << "Spine_Shoulder:" << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
							}
							if (rt == "Spine_Mid") {
								cout << "       Spine_Mid: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
								writeFile << "Spine_Mid" << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
							}
							if (rt == "Spine_Base") {
								cout << "       Spine_Base: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
								writeFile << "Spine_Base:" << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
							}
							if (rt == "Hip(Left)") {
								cout << "       Hip(Left): " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
								writeFile << "Hip(Left):" << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
							}
							if (rt == "Hip(Right)") {
								cout << "       Hip(Right): " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
								writeFile << "Hip(Right):" << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
							}
							}

					}
					writeFile << endl;
				}
				cout << count << " joints tracked" << endl << endl;
				
			
			}
		}
		myDepthFrame->Release();
		myBodyFrame->Release();
		delete[] bodyArr;

		if (waitKey(500) == VK_ESCAPE)
			break;
		//Sleep(1000);    //매초마다 느리게 업데이트

	}
	myBodyReader->Release();
	myDepthReader->Release();
	myBodySource->Release();
	myDepthSource->Release();
	mySensor->Close();
	mySensor->Release();

	writeFile.close();

	return  0;
}

const   string  get_name(int n)
{
	switch (n)
	{
	case 0:
		return "Spine_Base";
		break;
	case 1:
		return "Spine_Mid";
		break;
	case 2:
		return "Neck";
		break;
	case 3:
		return "Head";
		break;
	case 12:
		return "Hip(Left)";
		break;
	case 16:
		return "Hip(Right)";
		break;
	case 20:
		return "Spine_Shoulder";
		break;

	default:return "NULL";
	}
}
