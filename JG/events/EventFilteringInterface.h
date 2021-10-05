#pragma once

using RefID = UInt32;
using FormID = RefID;

// For plugins connecting to JG only.
// Credits: Copies the structure from NVSEArrayVarInterface.
struct JGEventHandlerInterface
{
	enum {
		kVersion = 1
	};

	// Each filter element is an instance of this class.
	struct FilterElem
	{
	protected:
		using FilterType = uint8_t;
		FilterType type;

		void Reset() { if (type == kType_String) { FormHeap_Free(str); type = kType_Ptr; ptr = nullptr; } }

	public:
		union //FilterTypes
		{
			void* ptr;
			FormID		formID;
			int         intVal;
			float       fltVal;
			char* str;
		};
		
		enum FilterTypeIndexes : FilterType
		{
			kType_Ptr = 0,
			kType_Form,
			kType_Int,
			kType_Float,
			kType_String
		};

		~FilterElem() { Reset(); }

		FilterElem() : type(kType_Ptr), ptr(nullptr) { }
		FilterElem(const char* _str) : type(kType_String) { str = CopyCString(_str); }
		FilterElem(float _num) : fltVal(_num), type(kType_Float) { }
		FilterElem(int _num) : intVal(_num), type(kType_Float) { }
		FilterElem(TESForm* _form) : formID(_form->GetId()), type(kType_Form) { }

		[[nodiscard]] FilterType GetType() const { return type; }

		[[nodiscard]] bool IsValid() const
		{
			if (GetType() == kType_Ptr && ptr == nullptr)
				return false;
			return true;
		}

		[[nodiscard]] const char* GetString() const { return type == kType_String ? str : nullptr; }
		[[nodiscard]] TESForm* GetTESForm() const { return type == kType_Form ? LookupFormByID(formID) : nullptr; }
		[[nodiscard]] FormID GetFormID() const { return type == kType_Form ? formID : NULL; }
		[[nodiscard]] int GetInt() const { return type == kType_Int ? intVal : 0; }
		[[nodiscard]] float GetFloat() const { return type == kType_Float ? fltVal : 0.0f; }
		[[nodiscard]] void* GetPointer() const { return type == kType_Ptr ? ptr : nullptr; }
	};

	struct FilterElemSet
	{
	protected:
		using FilterType = uint8_t;
		FilterType type;
		size_t numElems;

		void Reset() { if (type == kType_String) { FormHeap_Free(str); type = kType_Ptr; ptr = nullptr; } }

	public:
		union //FilterSetTypes
		{
			void** ptrSet;
			FormID*		formIDSet;
			int*         intSet;
			float*       fltSet;
			char** strSet;
		};

		enum FilterSetTypeIndexes : FilterType
		{
			kType_PtrSet = 0,
			kType_FormSet,
			kType_IntSet,
			kType_FloatSet,
			kType_StringSet
		};

		~FilterElemSet() { Reset(); }

		FilterElemSet() : type(kType_PtrSet), numElems(0), ptrSet(nullptr) { }
		/*
		FilterElemSet(char** _str, size_t _numElems) : type(kType_StringSet), numElems(0)
		{

			
			strSet = CopyCString(_str);
		}
		FilterElemSet(float _num) : fltSet(_num), type(kType_Float) { }
		FilterElemSet(int _num) : intVal(_num), type(kType_Float) { }
		FilterElemSet(TESForm* _form) : formID(_form->GetId()), type(kType_Form) { }
		*/

		[[nodiscard]] FilterType GetType() const { return type; }
		[[nodiscard]] size_t GetSize() const { return numElems; }


		[[nodiscard]] bool IsValid() const
		{
			if (GetType() == kType_PtrSet && ptrSet == nullptr)
				return false;
			return true;
		}

		[[nodiscard]] char** GetStringSet() const { return type == kType_StringSet ? strSet : nullptr; }
		//[[nodiscard]] TESForm* GetTESFormSet() const { return type == kType_Form ? LookupFormByID(formSet) : nullptr; }
		[[nodiscard]] FormID* GetFormIDSet() const { return type == kType_FormSet ? formIDSet : nullptr; }
		[[nodiscard]] int* GetIntSet() const { return type == kType_IntSet ? intSet : nullptr; }
		[[nodiscard]] float* GetFloatSet() const { return type == kType_FloatSet ? fltSet : nullptr; }
		[[nodiscard]] void** GetPointerSet() const { return type == kType_PtrSet ? ptrSet : nullptr; }
	};

};

using JGFilterElem = JGEventHandlerInterface::FilterElem;
using JGFilterSet = JGEventHandlerInterface::FilterElemSet;


#define UseHelperFunctions 1

#if UseHelperFunctions

#include <variant>

using JGFilterTypes = JGEventHandlerInterface::FilterElem::FilterTypeIndexes;
using JGFilterTypesVariant = std::variant<void*, FormID, int, float, std::string>;

JGFilterTypesVariant GetFilterElemAsVariant(JGFilterElem& filter_elem)
{	
	switch (filter_elem.GetType())
	{
	case JGFilterTypes::kType_Form:
		return filter_elem.formID;
	case JGFilterTypes::kType_Int:
		return filter_elem.intVal;
	case JGFilterTypes::kType_Float:
		return filter_elem.fltVal;
	case JGFilterTypes::kType_String:
		return filter_elem.str;
	case JGFilterTypes::kType_Ptr:
	default:
		return filter_elem.ptr;
	}
}

#include <unordered_set>

// If there are more than 1 item per filter, will search for any match.
// Example: can fill an IntSet with multiple EquipSlot codes, and if any are matched, then event handler will fire.
using VoidPtrSet = std::unordered_set<void*>;
using StringSet = std::unordered_set<std::string>;
using FloatSet = std::unordered_set<float>;
using IntSet = std::unordered_set<int>;
// Storing TESForm* can cause rare crashes due to dynamic refs, so use RefIDs.
using RefIDSet = std::unordered_set<RefID>;

using JGFilterSetTypes = JGEventHandlerInterface::FilterElemSet::FilterSetTypeIndexes;
using JGFilterSetVariant = std::variant<VoidPtrSet, RefIDSet, IntSet, FloatSet, StringSet>;
using JGFilterSetVariantArray = std::vector<JGFilterSetVariant>;

static_assert(std::variant_size_v<JGFilterTypesVariant> == std::variant_size_v<JGFilterSetVariant>);

#include <span>

template <typename FilterType>
std::unordered_set<FilterType> ConvertRawFilterToStdSet(FilterType* data, size_t size)
{
	std::span<FilterType> spanSet{ data, size };
	std::unordered_set<FilterType> stdSet;
	for (auto const elem : spanSet)
	{
		stdSet.emplace(elem);
	}
	
	// TODO: test if elements inside the new std::set remain valid when the raw set is destroyed.
	return stdSet;
}

JGFilterSetVariant GetRawFilterSetAsStdSet(JGFilterSet &filter_set)
{
	auto const size = filter_set.GetSize();
	JGFilterSetVariant setVariant;

	switch (filter_set.GetType())
	{
	case JGFilterSetTypes::kType_FormSet:
		setVariant = ConvertRawFilterToStdSet(filter_set.formIDSet, size);
		break;
	case JGFilterSetTypes::kType_IntSet:
		setVariant = ConvertRawFilterToStdSet(filter_set.intSet, size);
		break;
	case JGFilterSetTypes::kType_FloatSet:
		setVariant = ConvertRawFilterToStdSet(filter_set.fltSet, size);
		break;
	case JGFilterSetTypes::kType_StringSet:
	{
		std::span rawStrSpan{ filter_set.strSet, size };
		StringSet strSet;
		for (auto const rawStr : rawStrSpan)
		{
			std::string const newStr = rawStr;
			strSet.emplace(newStr);
		}
		setVariant = strSet;
		break;
	}
	case JGFilterSetTypes::kType_PtrSet:
	default:
		setVariant = ConvertRawFilterToStdSet(filter_set.ptrSet, size);
	}
	
	return setVariant;
}

#if NULL //examples:
FilterTypeSets testFilter1 = { IntSet {5, 0x7} };
FilterTypeSets testFilter2 = { StringSet {"testStr", "testStr2", "tt"} };
FilterTypeSetArray testFilters = { testFilter1, testFilter2 };
#endif


/*
bool ArrayAPI::InternalElemToPluginElem(const ArrayElement* src, NVSEArrayVarInterface::Element* out)
{
	//...
	switch (src->DataType())
	{
	case kDataType_Numeric:
		*out = src->m_data.num;
		break;
	case kDataType_Form:
		*out = LookupFormByID(src->m_data.formID);
		break;
	case kDataType_Array:
	{
		ArrayID arrID = 0;
		src->GetAsArray(&arrID);
		*out = (NVSEArrayVarInterface::Array*)arrID;
		break;
	}
	case kDataType_String:
		*out = src->m_data.GetStr();
		break;
	}
	return true;
}
*/

#endif 

#undef UseHelperFunctions




// The following filter values are ignored by GenericEventFilters (unfiltered).
enum IgnoreFilter_Values
{
	// For RefID filters
	kIgnFilter_xMarkerID = 0x3B,
	kIgnFilter_RefID = kIgnFilter_xMarkerID,
	kIgnFilter_FormID = kIgnFilter_xMarkerID,
	kIgnFilter_Form = kIgnFilter_xMarkerID,

	// For int filters
	kIgnFilter_Int = -1,
};




class EventHandlerFilterBase
{
	//== NOTE:
	// The following private methods are used for implementing the filters on the JG side of things.
	// There is no reason to use them if using the JG exports.
	// END NOTE ==

	// Framework passes the objects to add to filter here
	virtual ~EventHandlerFilterBase() = default;

	// Used to filter out "-1" int codes, transform a form-list TESForm* into a bunch of TESForm*s, etc.
	// Alternatively, can set up one's own data structures here.
	virtual void SetUpFiltering() = 0;

	// Used by the framework to check if the Nth Gen filter equals the passed value set. Useful to avoid adding the same event repeatedly
	virtual bool IsGenFilterEqual(UInt32 filterNum, FilterTypeSets const& cmpFilterSet) = 0;
	
	// Used to check if the Nth Gen filter equals the passed value set.
	// ALL default-value Gen filters are said to be "equal".
	// Useful to mass-remove events by using default filters.
	virtual bool IsGenFilterEqualAlt(UInt32 filterNum, FilterTypeSets const& cmpFilterSet) = 0;
	
public:


	// Checks if an object is in the filter, recommended to use a fast lookup data structure
	virtual bool IsInFilter(UInt32 filterNum, FilterTypes toSearch) = 0;

	// Checks if the base form is in the form filter.
	// Returns false if the filter is not form-type.
	virtual bool IsBaseInFilter(UInt32 filterNum, RefID toSearch) = 0;
	virtual bool IsBaseInFilter(UInt32 filterNum, TESForm* toSearch) = 0;

	// Inserts the desired element to the Nth filter.
	virtual bool InsertToFilter(UInt32 filterNum, FilterTypes toInsert) = 0;

	// Deletes an element from the Nth filter
	virtual bool DeleteFromFilter(UInt32 filterNum, FilterTypes toDelete) = 0;

	// Returns if the filter is empty
	virtual bool IsFilterEmpty(UInt32 filterNum) = 0;

	// Hope a UInt32 is large enough.
	virtual UInt32 GetNumFilters() = 0; // return filtersArr.size(); }
	virtual UInt32 GetNumGenFilters() = 0; // { return genFiltersArr.size(); }

	FilterTypeSets* GetNthFilter(UInt32 numFilter)
	{
		if (numFilter >= filtersArr.size()) return nullptr;
		return &filtersArr[numFilter];
	}
	FilterTypeSets* GetNthGenFilter(UInt32 numFilter)
	{
		if (numFilter >= genFiltersArr.size()) return nullptr;
		return &genFiltersArr[numFilter];
	}
};


class EventContainerInterface
{
	using FlagType = uint16_t;

public:
	virtual ~EventContainerInterface() = default;

	// Returns true/false if event was registered successfully (not a duplicate).
	bool virtual RegisterEvent(Script* script, /*void***/ filters);

	// Returns the amount of events that were cleared for the associated script.
	size_t virtual RemoveEvent(Script* script, /*void***/ filters);

	enum EventFlags : FlagType
	{
		eFlag_FlushOnLoad = 1 << 0,
	};
	[[nodiscard]] virtual EventFlags GetFlags() const = 0;
};


EventContainerInterface* (_cdecl* CreateEventHandler)(const char* EventName, UInt8 maxArgs, UInt8 maxFilters, void* (__fastcall* FilterConstructor)(void**, UInt32));
void(__cdecl* FreeEventHandler)(EventContainerInterface*& toRemove);


// All event filters should inherit from this.
// Used by EventInformation class to ensure that the declared filter type is the same when registering/removing events.
struct EventFilter_Base
{
	virtual ~EventFilter_Base() = default;
	[[nodiscard]] virtual FilterTypeSetArray ToFilter() const = 0;
};

