#include "Playground.h"
#include "windows.h"

extern "C"
{
	INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
	{
		//std::list<Real> angles;
		//for (int i = 0; i < 360; i++)
		//{
		//	Vector3 normal = Quaternion(Radian(Degree(i)), Vector3::UNIT_Z) * Vector3::UNIT_X;
		//	auto dot = Vector3::UNIT_Y.dotProduct(normal);
		//	Real angl = Math::ACos(dot / normal.length()).valueAngleUnits();// <= btRadians(PLAYER_MAX_SLOPE);
		//	angles.push_back(angl);
		//}

		Playground app;

		try
		{
			app.go();
		}
		catch(Ogre::Exception& e)
		{
			MessageBox(NULL, (String("Error code: ") + e.getDescription()).c_str(), "Exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		}
	}
}