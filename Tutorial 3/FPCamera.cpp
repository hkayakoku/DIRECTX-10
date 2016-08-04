#include "FPCamera.h"

FPCamera::FPCamera(D3DXVECTOR3 startPos) : m_position(startPos), m_yaw(0), m_pitch(0), m_roll(0)
{
	// Start with an orthagonal camera axis
	m_up = D3DXVECTOR3(0.0f, 0.0f, -5.0f);
	m_look = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_right = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	kCameraMovementSpeed = 0.4f;
}

FPCamera::~FPCamera(void)
{
}

void FPCamera::CalculateViewMatrix(D3DXMATRIX *viewMatrix)
{
	/* Start with our camera axis pointing down z
	An alternative method is to just keep adjusting our axis but if we do that the
	axis can start to lose its orthogonal shape (due to floating point innacuracies).
	This could be solved by rebuilding the orthogonal shape each time with the following:
	1. normalising the look vector
	2. creating the up vector from the cross product of the look and the right
	3. normalising up
	4. creating the right vector from the cross product of the look and the up
	5. normalising right
	*/

	m_up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_look = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

	// Yaw is rotation around the y axis (m_up)
	// Create a matrix that can carry out this rotation

	D3DXMATRIX yawMatrix;
	D3DXMatrixRotationAxis(&yawMatrix, &m_up, m_yaw);
	// To apply yaw we rotate the m_look & m_right vectors about the m_up vector (using our yaw matrix)
	D3DXVec3TransformCoord(&m_look, &m_look, &yawMatrix);
	D3DXVec3TransformCoord(&m_right, &m_right, &yawMatrix);

	// Pitch is rotation around the x axis (m_right)
	// Create a matrix that can carry out this rotation
	D3DXMATRIX pitchMatrix;
	D3DXMatrixRotationAxis(&pitchMatrix, &m_right, m_pitch);

	// To apply pitch we rotate the m_look and m_up vectors about the m_right vector (using our pitch matrix)
	D3DXVec3TransformCoord(&m_look, &m_look, &pitchMatrix);
	D3DXVec3TransformCoord(&m_up, &m_up, &pitchMatrix);

	// Roll is rotation around the z axis (m_look)
	// Create a matrix that can carry out this rotation
	D3DXMATRIX rollMatrix;
	D3DXMatrixRotationAxis(&rollMatrix, &m_look, m_roll);

	// To apply roll we rotate up and right about the look vector (using our roll matrix)
	// Note: roll only really applies for things like aircraft unless you are implementing lean
	D3DXVec3TransformCoord(&m_right, &m_right, &rollMatrix);
	D3DXVec3TransformCoord(&m_up, &m_up, &rollMatrix);

	// Build the view matrix from the transformed camera axis
	D3DXMatrixIdentity(viewMatrix);

	viewMatrix->_11 = m_right.x; viewMatrix->_12 = m_up.x; viewMatrix->_13 = m_look.x;
	viewMatrix->_21 = m_right.y; viewMatrix->_22 = m_up.y; viewMatrix->_23 = m_look.y;
	viewMatrix->_31 = m_right.z; viewMatrix->_32 = m_up.z; viewMatrix->_33 = m_look.z;

	viewMatrix->_41 = -D3DXVec3Dot(&m_position, &m_right);
	viewMatrix->_42 = -D3DXVec3Dot(&m_position, &m_up);
	viewMatrix->_43 = -D3DXVec3Dot(&m_position, &m_look);
}



bool FPCamera::buttonListener()
{
	if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('X') & 0x8000) {
		//decreaseX();
		////setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('Y') & 0x8000) {
		//decreaseY();
		////setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_SHIFT) & GetAsyncKeyState('Z') & 0x8000) {
		//decreaseZ();
		////setMatrices();
		return true;
	}

	if (GetAsyncKeyState('X') & 0x8000) {
		//increaseX();

		return true;
		//wchar_t  str[256];
		//wsprintf(str, L"It works! - number: %d \n", getZ() );
		//OutputDebugString(str);

		//setMatrices();

	}

	if (GetAsyncKeyState('Y') & 0x8000) {
		//increaseY();
		//setMatrices();

		return true;
	}
	if (GetAsyncKeyState('Z') & 0x8000) {
		//increaseZ();
		//setMatrices();
		return true;
	}




	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		//increaseZ();
		//setMatrices();
		MoveForward();
		return true;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		//decreaseZ();
		//setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		//increaseRotate();
		//setMatrices();
		return true;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		//decreaseRotate();
		//setMatrices();
		return true;
	}

	return false;
}

void FPCamera::printCoordinate()
{
	char  str[80];
	//sprintf(str, "X: %f Y: %f Y: %f rot: %f \n", getX(), getY(), getZ(), getRotate());
	OutputDebugString(str);
}


// Yaw - rotation around y axis
void FPCamera::Yaw()
{
	m_yaw += kCameraMovementSpeed;
	m_yaw = RestrictAngleTo360Range(m_yaw);
}

// Pitch - rotation around x axis
void FPCamera::Pitch()
{
	m_pitch += kCameraMovementSpeed;
	m_pitch = RestrictAngleTo360Range(m_pitch);
}

// Roll - rotation around z axis
// Note: normally only used for aircraft type cameras rather than land based ones
void FPCamera::Roll()
{
	m_roll += kCameraMovementSpeed;
	m_roll = RestrictAngleTo360Range(m_roll);
}

// Keep the angle in the range 0 to 360 (2*PI)
float FPCamera::RestrictAngleTo360Range(float angle) const
{
	while (angle>2 * D3DX_PI)
		angle -= 2 * D3DX_PI;

	while (angle<0)
		angle += 2 * D3DX_PI;

	return angle;
}