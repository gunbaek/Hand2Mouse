#ifndef _BLOBLABELING_H_
#define _BLOBLABELING_H_

#pragma once

#include<opencv\cv.h>
#include<opencv\highgui.h>


typedef struct 
{
	bool	bVisitedFlag;
	CvPoint ptReturnPoint;
} Visited;

class  CBlobLabeling
{
public:
	void BlobSmallSizeConstraint(int nWidth, int nHeight);
private:
//	void _BlobSmallSizeConstraint(int nWidth, int nHeight,CvRect *rect, int *label, int nRecNumber);
	void _BlobSmallSizeConstraint(int nWidth, int nHeight,CvRect *rect, int *nRecNumber);
public:
	void BlobBigSizeConstraint(int nWidth, int nHeight);
private:
//	void _BlobBigSizeConstraint(int nWidth, int nHeight,CvRect *rect, int *label, int nRecNumber);
	void _BlobBigSizeConstraint(int nWidth, int nHeight,CvRect *rect, int *nRecNumber);


public:
	CBlobLabeling(void);
public:
	~CBlobLabeling(void);

public:
	IplImage*	m_Image;				// ���̺��� ���� �̹���
	int			m_nThreshold;			// ���̺� ������Ȧ�� ��
	Visited*	m_vPoint;				// ���̺��� �湮����
	int			m_nBlobs;				// ���̺��� ����
	CvRect*		m_recBlobs;				// �� ���̺� ����


public:
	// ���̺� �̹��� ����
	void		SetParam(IplImage* image, int nThreshold);

	// ���̺�(����)
	void		DoLabeling();

private:
	// ���̺�(����)
	int		 Labeling(IplImage* image, int nThreshold);

	// ����Ʈ �ʱ�ȭ
	void	 InitvPoint(int nWidth, int nHeight);
	void	 DeletevPoint();

	// ���̺� ��� ���
	void	 DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight);

	// ���̺�(���� �˰���)
	int		_Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThreshold);
	
	// _Labling ���� ��� �Լ�
	int		__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY );
	int		__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel);
};

#endif