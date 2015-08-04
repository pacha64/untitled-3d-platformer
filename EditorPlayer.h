#pragma once
#include "stdafx.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include "balance.h"
#include "WorldGenerator.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Physics.h"
#include "TextRenderer.h"
#include "SoundManager.h"
#include "DebugDrawer.h"
#include "WorldEditor.h"
#include "MapFormat.h"

class GameGUI;
class Player;

class EditorPlayer
{
public:
	EditorPlayer(Player*, GameGUI*, Camera*, SceneManager*, WorldEditor*);
	~EditorPlayer(void);
	
	void onPlayerToggle();
	void update(const FrameEvent&, OIS::Keyboard* keyboard, OIS::Mouse* mouse);
protected:
	void changeCameraFreeRoaming();
	void changeCameraOrbit(bool resetZoom = false);
	void changeCameraOrbit(Vector3 position, int length);

	void onStartEditing();
	void onFinishEditing();

	void updateKeyboardNumber(char* keystateNew, char* keystateOld, int& number);


	class Matrix3D
	{
		std::vector<Platform*> vector;
		Vector3 origin;
		Degree rotation;
		unsigned int margin;

	public:
		double maxX, maxY, maxZ;
		Matrix3D (Vector3 origin, int margin)
			: maxX(1),
			maxY(1),
			maxZ(1),
			origin(origin),
			margin(margin)
		{
			refreshNumber();
			refreshPosition();
		}
		
		~Matrix3D()
		{
		}

		void deleteAllPlatforms()
		{
			for (int i = 0; i < vector.size(); i++)
			{
				Matrix3D::editor->deletePlatform(vector[i]);
			}
		}
		void refresh()
		{
			refreshNumber();
			refreshPosition();
		}
		void refreshNumber();
		void refreshPosition();

		bool canAddMorePlatforms(int axis);

		void setOrigin(Vector3 vector)
		{
			this->origin = vector;
			refreshPosition();
		}

		void setMargin(int margin)
		{
			if (margin < EDITOR_MIN_MARGIN_MATRIX) margin = EDITOR_MIN_MARGIN_MATRIX;
			this->margin = margin;
			refreshPosition();
		}

		void setRotation(Degree rotation)
		{
			this->rotation = rotation;
			for (int i = 0; i < vector.size(); i++)
			{
				vector[i]->_setRotate(this->rotation);
			}
		}
		Degree getRotation()
		{
			return this->rotation;
		}

		Platform* operator()(int x, int y, int z)
		{
			if (x >= maxX || y>= maxY || z >= maxZ)
				throw 0; // ouch
			return vector[
				maxX * maxY * z +
				maxX * y +
				x
			];
		}


		void addX()
		{
			maxX++;
			if (maxX > INSERT_MATRIX_MAX_SIZE) maxX = INSERT_MATRIX_MAX_SIZE;
			refresh();
		}
		void removeX()
		{
			maxX--;
			if (maxX == 0) maxX = 1;
			refresh();
		}
		void addY()
		{
			maxY++;
			if (maxY > INSERT_MATRIX_MAX_SIZE) maxY = INSERT_MATRIX_MAX_SIZE;
			refresh();
		}
		void removeY()
		{
			maxY--;
			if (maxY == 0) maxY = 1;
			refresh();
		}
		void addZ()
		{
			maxZ++;
			if (maxZ > INSERT_MATRIX_MAX_SIZE) maxZ = INSERT_MATRIX_MAX_SIZE;
			refresh();
		}
		void removeZ()
		{
			maxZ--;
			if (maxZ == 0) maxZ = 1;
			refresh();
		}
		static WorldEditor* editor;
	};

	class Path
	{
		Vector3 origin;
		double margin;
		double height;
		std::vector<Platform*> platforms;
		Degree previousRotation, currentRotation;
		double currentMargin, currentHeight;
	public:
		Path(Vector3 origin, int height, int margin)
		{
			this->origin = origin;
			this->height = height;
			this->margin = margin;
			
			currentRotation = Degree(0);
			previousRotation = currentRotation;
			currentMargin = margin;
			currentHeight = height;

			// not implemented
			platforms.push_back(Matrix3D::editor->newPlatform(
				origin,
				Degree(0)//,
//				Vector3(1, 1, 1)
			));
		}

		~Path()
		{
		}

		void deleteAllPlatforms()
		{
			for (int i = 0; i < platforms.size(); i++)
			{
				Matrix3D::editor->deletePlatform(platforms[i]);
			}
		}

		Platform* lastPlatform()
		{
			return platforms[platforms.size() - 1];
		}
		void appendPlatform();
		void setRotation(Degree rotation);
		void setMargin(int margin);
		void setHeight(int height);

		void setDefaultMargin(int margin);
		void setDefaultHeight(int height);

		void addRotation()
		{
			setRotation(currentRotation + Degree(15));
		}
		void reduceRotation()
		{
			setRotation(currentRotation - Degree(15));
		}
		void addHeight()
		{
			setHeight(currentHeight + 25);
		}
		void reduceHeight()
		{
			setHeight(currentHeight - 25);
		}

		void deleteOne()
		{
			if (platforms.size() > 1)
			{
				previousRotation = platforms[platforms.size() - 1]->getBox()->getNode()->_getDerivedOrientation().getYaw();
				Matrix3D::editor->deletePlatform(platforms[platforms.size() - 1]);
				platforms.erase(platforms.begin() + platforms.size() - 1);
			}
		}
	};

	GameGUI* mGameGUI;
	WorldEditor* mEditor;
	Camera* mCamera;
	SceneNode* mMainNode;
	SceneNode* mCameraPivot;
	SceneNode* mCameraYaw;
	SceneNode* mCameraPitch;
	SceneNode* mCameraHolder;
	Player* mPlayerInstance;
	std::vector<Platform*> mCurrentModifyingPlatform, mCurrentModifyingPlatformHelperBuffer, mAddingBufferHelper;
	std::vector<Vector3> mModifyingPlatformStartPos, mModifyingPlatformLockStartPos;
	std::vector<Degree> mModifyingPlatformLockStartRotation;
	OIS::MouseState mOldMouseState;
	Matrix3D* mMatrix;
	Path* mPath;
	int mCameraRoaming, mAddingStep;
	Vector3 mAddingVectorHelper, mAddingVectorHelper2;
	char mKeystateOld[256];
	bool mLockX, mLockY, mLockZ, mRotate, mModifyingOrigin, mMovementAbsolute, mModifyingSpeed, mAddingMatrix, mAddingPath;
	int mLockNumber;
};

