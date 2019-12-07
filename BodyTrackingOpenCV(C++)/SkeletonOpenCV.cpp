#define S_OK ((HRESULT)0L)
//Standard Library
#include <iostream>
#include <stdio.h>
//OpenCV Header
#include <opencv2\opencv.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<opencv2\video\background_segm.hpp>
#include <Kinect.h>
using namespace cv;
using namespace std;
// Kinect for Windows SDK Header


const   string  get_name(int n);    //�����̸� ����
int main(void)
{
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
	myDescription->get_Width(&width);   //���� �����Ϳ� �� �����͸� �غ�

	IBodyFrame* myBodyFrame = nullptr;
	IDepthFrame* myDepthFrame = nullptr;
	Mat img16(height, width, CV_16UC1); //���� �̹��� ǥ�� �غ�
	Mat img8(height, width, CV_8UC1);

	while (1)
	{
		while (myDepthReader->AcquireLatestFrame(&myDepthFrame) != S_OK);
		myDepthFrame->CopyFrameDataToArray(width * height, (UINT16*)img16.data);
		img16.convertTo(img8, CV_8UC1, 255.0 / 4500);
		imshow("Depth Img", img8);  //�ɵ� �̹��� ��ȯ �� ���÷���

		while (myBodyReader->AcquireLatestFrame(&myBodyFrame) != S_OK);

		int myBodyCount = 0;
		IBody * *bodyArr = nullptr;
		myBodySource->get_BodyCount(&myBodyCount);//ī�޶�� �Կ� �� �� ��ü ��
		bodyArr = new IBody * [myBodyCount];
		for (int i = 0; i < myBodyCount; i++)   //bodyArr�ʱ�ȭ
			bodyArr[i] = nullptr;

		myBodyFrame->GetAndRefreshBodyData(myBodyCount, bodyArr);

		for (int i = 0; i < myBodyCount; i++)   //6 �� ��ȸ
		{
			BOOLEAN     result = false;
			if (bodyArr[i]->get_IsTracked(&result) == S_OK && result)   //�� ����� �����Ǿ����� Ȯ��
			{
				cout << "Body " << i << " tracked!" << endl;

				int count = 0;
				Joint   jointArr[JointType_Count];
				bodyArr[i]->GetJoints(JointType_Count, jointArr);    //�� ����� ���� ���� �����͸� ������
				for (int j = 0; j < JointType_Count; j++)
				{
					if (jointArr[j].TrackingState != TrackingState_Tracked) //���� �� ������ ǥ�õǴ��� Ȯ����
						continue;
					string  rt = get_name(jointArr[j].JointType);   //�����̸� ���
					if (rt != "NULL")   //����Ʈ ���� ���

					{
						count++;
						cout << "   " << rt << " tracked" << endl;
						if (rt == "��")
							cout << "       ����ǥ: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
						if (rt == "�߾Ӿ��")
							cout << "       �߾Ӿ����ǥ: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
						if (rt == "���ʾ��")
							cout << "       ���ʾ����ǥ: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
						if (rt == "�����ʾ��")
							cout << "       �����ʾ����ǥ: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
						if (rt == "�߾�ô��")
							cout << "       �߾�ô����ǥ: " << jointArr[j].Position.X << "," << jointArr[j].Position.Y << "," << jointArr[j].Position.Z << endl;
					}

				}
				cout << count << " joints tracked" << endl << endl;
			}
		}
		myDepthFrame->Release();
		myBodyFrame->Release();
		delete[] bodyArr;

		if (waitKey(30) == VK_ESCAPE)
			break;
		//Sleep(1000);    //���ʸ��� ������ ������Ʈ

	}
	myBodyReader->Release();
	myDepthReader->Release();
	myBodySource->Release();
	myDepthSource->Release();
	mySensor->Close();
	mySensor->Release();

	return  0;
}

const   string  get_name(int n)
{
	switch (n)
	{
	case 2:
		return "��";
		break;
	case 20:
		return  "�߾� ���";
		break;
	case 4:
		return  "���ʾ��";
		break;
	case 8:
		return  "������ ���";
		break;
	case 1:
		return  "�߾� ô��";
		break;

	default:return "NULL";
	}
}