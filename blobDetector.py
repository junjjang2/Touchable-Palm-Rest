import cv2
import numpy as np

__test__ = False

class BlobDetector:
    def __init__(self, rows, cols) -> None:
        self.image = np.zeros(shape=(rows, cols))

    def process(self, signal):
        self.image = np.copy(signal)
        # detect blobs
        keypoints = self.blobDetection(self.image)
        
        if __test__:
            testImage = np.copy(self.image)
            testImage = cv2.drawKeypoints(testImage, keypoints, np.array([]), (0, 0, 255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

        blob_centers = [(int(keypoint.pt[0]), int(keypoint.pt[1])) for keypoint in keypoints]
        if __test__:
            for center in blob_centers:
                cv2.circle(testImage, center, 2, (0, 255, 0), -1)

        if __test__:
            cv2.imshow('Blob Image', testImage)
            cv2.waitKey(0)
            cv2.destroyAllWindows()
        
        return blob_centers

    def blobDetection(self, image):
        # Setup SimpleBlobDetector parameters.
        params = cv2.SimpleBlobDetector_Params()
        params.filterByArea = True
        params.minArea = 100
        params.maxArea = 10000
        # params.filterByCircularity = False
        # params.filterByInertia = False
        # params.filterByConvexity = False
        # params.filterByColor = False

        # Create a detector with the parameters
        detector = cv2.SimpleBlobDetector_create(params)
        keypoints = detector.detect(image)
        return keypoints
    

if __name__ == '__main__':
    __test__ = True
    print("Blob Detector Test")
    blobtestimg = cv2.imread('Sample/BlobTest.jpg', cv2.IMREAD_GRAYSCALE)
    row, col = blobtestimg.shape
    blobDetector = BlobDetector(row, col)
    blobDetector.process(blobtestimg)
    print('Done')