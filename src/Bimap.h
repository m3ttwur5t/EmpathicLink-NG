#include <stdexcept>

template <typename KeyType, typename ValueType>
class BiMap
{
private:
	std::map<KeyType, ValueType> forwardMap;
	std::map<ValueType, KeyType> reverseMap;

public:
	constexpr std::map<KeyType, ValueType> GetForwardMap() const
	{
		return forwardMap;
	}
	constexpr std::map<KeyType, ValueType> GetReverseMap() const
	{
		return reverseMap;
	}

	void insert(KeyType key, ValueType value)
	{
		forwardMap[key] = value;
		reverseMap[value] = key;
	}

	ValueType getValue(KeyType key)
	{
		if (!forwardMap.contains(key)) {
			throw std::out_of_range("Key not found");
		}
		return forwardMap[key];
	}
	ValueType getValueOrNull(KeyType key)
	{
		if (!forwardMap.contains(key)) {
			return ValueType();
		}
		return forwardMap[key];
	}

	KeyType getKey(ValueType value)
	{
		if (!reverseMap.contains(value)) {
			throw std::out_of_range("Value not found");
		}
		return reverseMap[value];
	}

	KeyType getKeyOrNull(ValueType value)
	{
		if (!reverseMap.contains(value)) {
			return KeyType();
		}
		return reverseMap[value];
	}

	bool containsKey(KeyType key)
	{
		return forwardMap.contains(key);
	}

	bool containsValue(ValueType value)
	{
		return reverseMap.contains(value);
	}

	void eraseKey(KeyType key)
	{
		if (forwardMap.contains(key)) {
			reverseMap.erase(forwardMap[key]);
			forwardMap.erase(key);
		}
	}

	void eraseValue(ValueType value)
	{
		if (reverseMap.contains(value)) {
			forwardMap.erase(reverseMap[value]);
			reverseMap.erase(value);
		}
	}

	void clear()
	{
		forwardMap.clear();
		reverseMap.clear();
	}

	size_t size()
	{
		return forwardMap.size();
	}

	bool empty()
	{
		return forwardMap.empty();
	}
};
