#ifndef _TECHNIQUE_H_
#define _TECHNIQUE_H_

#include "AntiDebug.h"
#include <memory>
#include <list>
#include <map>

class Technique : public AntiDebug
{
public:
	Technique(const std::string name, const std::shared_ptr<AntiDebug> &parent = nullptr)
		: AntiDebug(name, parent)
	{}
	virtual ~Technique() {}

	virtual bool IsGroup() const { return false; }
	virtual void AddTechnique(std::shared_ptr<AntiDebug>) {}
	virtual void AddParameter(std::string &name, ParamValue &value) { m_mParameters.insert(std::make_pair(name, value)); }
	virtual bool Check() const = 0;
	virtual bool IsEmpty() const { return false; }

protected:
	std::map<std::string, ParamValue> m_mParameters;
};

class TechniqueGroup : public AntiDebug
{
public:
	TechniqueGroup(const std::string name, const std::shared_ptr<AntiDebug> &parent = nullptr)
		: AntiDebug(name, parent)
	{}
	virtual ~TechniqueGroup() {}

	virtual bool IsGroup() const { return true; }
	virtual void AddTechnique(std::shared_ptr<AntiDebug> technique) { m_lstChildren.push_back(technique); }
	virtual void AddParameter(std::string &name, ParamValue &value) {}
	virtual bool Check() const;
	virtual bool IsEmpty() const { return m_lstChildren.empty(); }

private:
	std::list<std::shared_ptr<AntiDebug>> m_lstChildren;
};

#endif // _TECHNIQUE_H_
