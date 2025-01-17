#pragma once

#include "lc_math.h"
#include "lc_array.h"

enum class lcObjectType
{
	Piece,
	Camera,
	Light
};

template<typename T>
struct lcObjectKey
{
	lcStep Step;
	T Value;
};

template<typename T>
class lcObjectProperty
{
public:
	explicit lcObjectProperty(const T& DefaultValue)
		: mValue(DefaultValue)
	{
		ChangeKey(mValue, 1, true);
	}

	operator const T&() const
	{
		return mValue;
	}

	int GetSize() const
	{
		return static_cast<int>(mKeys.size());
	}

	bool IsEmpty() const
	{
		return mKeys.empty();
	}

	void Update(lcStep Step)
	{
		mValue = CalculateKey(Step);
	}

	void Reset()
	{
		mKeys.clear();
		ChangeKey(mValue, 1, true);
	}

	void Reset(const T& Value)
	{
		mValue = Value;
		Reset();
	}

	void ChangeKey(const T& Value, lcStep Step, bool AddKey);
	void InsertTime(lcStep Start, lcStep Time);
	void RemoveTime(lcStep Start, lcStep Time);

	void Save(QTextStream& Stream, const char* ObjectName, const char* VariableName) const;
	bool Load(QTextStream& Stream, const QString& Token, const char* VariableName);
	void SaveKeysLDraw(QTextStream& Stream, const char* ObjectName, const char* VariableName) const;
	void LoadKeysLDraw(QTextStream& Stream);

protected:
	const T& CalculateKey(lcStep Step) const;

	T mValue;
	std::vector<lcObjectKey<T>> mKeys;
};

struct lcObjectSection
{
	lcObject* Object = nullptr;
	quint32 Section = 0;
};

struct lcPieceInfoRayTest
{
	const PieceInfo* Info = nullptr;
	lcMatrix44 Transform;
	lcVector3 Plane;
};

struct lcObjectRayTest
{
	lcCamera* ViewCamera;
	bool PiecesOnly;
	bool IgnoreSelected;
	lcVector3 Start;
	lcVector3 End;
	float Distance = FLT_MAX;
	lcObjectSection ObjectSection;
	lcPieceInfoRayTest PieceInfoRayTest;
};

struct lcObjectBoxTest
{
	lcCamera* ViewCamera;
	lcVector4 Planes[6];
	lcArray<lcObject*> Objects;
};

#define LC_OBJECT_TRANSFORM_MOVE_X    0x001
#define LC_OBJECT_TRANSFORM_MOVE_Y    0x002
#define LC_OBJECT_TRANSFORM_MOVE_Z    0x004
#define LC_OBJECT_TRANSFORM_MOVE_XYZ (LC_OBJECT_TRANSFORM_MOVE_X | LC_OBJECT_TRANSFORM_MOVE_Y | LC_OBJECT_TRANSFORM_MOVE_Z)
#define LC_OBJECT_TRANSFORM_ROTATE_X  0x010
#define LC_OBJECT_TRANSFORM_ROTATE_Y  0x020
#define LC_OBJECT_TRANSFORM_ROTATE_Z  0x040
#define LC_OBJECT_TRANSFORM_ROTATE_XYZ (LC_OBJECT_TRANSFORM_ROTATE_X | LC_OBJECT_TRANSFORM_ROTATE_Y | LC_OBJECT_TRANSFORM_ROTATE_Z)
#define LC_OBJECT_TRANSFORM_SCALE_X   0x100
#define LC_OBJECT_TRANSFORM_SCALE_Y   0x200
#define LC_OBJECT_TRANSFORM_SCALE_Z   0x400
#define LC_OBJECT_TRANSFORM_SCALE_XYZ (LC_OBJECT_TRANSFORM_SCALE_X | LC_OBJECT_TRANSFORM_SCALE_Y | LC_OBJECT_TRANSFORM_SCALE_Z)

class lcObject
{
public:
	lcObject(lcObjectType ObjectType);
	virtual ~lcObject();

	lcObject(const lcObject&) = delete;
	lcObject(lcObject&&) = delete;
	lcObject& operator=(const lcObject&) = delete;
	lcObject& operator=(lcObject&&) = delete;

public:
	bool IsPiece() const
	{
		return mObjectType == lcObjectType::Piece;
	}

	bool IsCamera() const
	{
		return mObjectType == lcObjectType::Camera;
	}

	bool IsLight() const
	{
		return mObjectType == lcObjectType::Light;
	}

	lcObjectType GetType() const
	{
		return mObjectType;
	}

	virtual bool IsSelected() const = 0;
	virtual bool IsSelected(quint32 Section) const = 0;
	virtual void SetSelected(bool Selected) = 0;
	virtual void SetSelected(quint32 Section, bool Selected) = 0;
	virtual bool IsFocused() const = 0;
	virtual bool IsFocused(quint32 Section) const = 0;
	virtual void SetFocused(quint32 Section, bool Focused) = 0;
	virtual quint32 GetFocusSection() const = 0;

	virtual quint32 GetAllowedTransforms() const = 0;
	virtual lcVector3 GetSectionPosition(quint32 Section) const = 0;
	virtual void RayTest(lcObjectRayTest& ObjectRayTest) const = 0;
	virtual void BoxTest(lcObjectBoxTest& ObjectBoxTest) const = 0;
	virtual void DrawInterface(lcContext* Context, const lcScene& Scene) const = 0;
	virtual void RemoveKeyFrames() = 0;
	virtual QString GetName() const = 0;

private:
	lcObjectType mObjectType;
};
