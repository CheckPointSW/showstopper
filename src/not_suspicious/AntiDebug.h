#ifndef _ANTI_DEBUG_H_
#define _ANTI_DEBUG_H_

#include <memory>
#include <string>
#include <variant>

enum ParamType { Dword, Qword, Real, String, };
using ParamValue = std::variant<std::uint32_t, std::uint64_t, double, std::string>;

class AntiDebug
{
public:
	AntiDebug(const std::string name, const std::shared_ptr<AntiDebug> &parent = nullptr)
		: m_sName(name)
		, m_wpParent(parent)
	{}
	virtual ~AntiDebug() {}

	virtual bool IsGroup() const = 0;
	virtual void AddTechnique(std::shared_ptr<AntiDebug>) = 0;
	virtual void AddParameter(std::string &name, ParamValue &value) = 0;
	virtual bool Check() const = 0;
	virtual bool IsEmpty() const = 0;

	const std::string GetName() const { return m_sName; }

protected:
	mutable std::weak_ptr<AntiDebug> m_wpParent;

private:
	const std::string m_sName;
};

#endif // _ANTI_DEBUG_H_
