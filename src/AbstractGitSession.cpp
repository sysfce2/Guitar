#include "AbstractGitSession.h"
#include "GitCommandCache.h"
#include "common/joinpath.h"

struct AbstractGitSession::GitCache {
	GitCommandCache command_cache;
};

struct AbstractGitSession::Private {
	std::shared_ptr<AbstractGitSession::GitCache> cache;
	AbstractGitSession::Info info;
	AbstractGitSession::Var var;
};

void AbstractGitSession::insertIntoCommandCache(const QString &key, const std::vector<char> &value)
{
	if (m->cache->command_cache) {
		m->cache->command_cache.insert(key, m->var.result);
	}
}

std::vector<char> *AbstractGitSession::findFromCommandCache(const QString &key)
{
	if (m->cache->command_cache) {
		return m->cache->command_cache.find(key);
	}
	return nullptr;
}

AbstractGitSession::AbstractGitSession()
	: m(new Private)
{
	m->cache = std::make_shared<GitCache>();
}

AbstractGitSession::~AbstractGitSession()
{
	delete m;
}

AbstractGitSession::Var &AbstractGitSession::var()
{
	return m->var;
}

const AbstractGitSession::Var &AbstractGitSession::var() const
{
	return m->var;
}

AbstractGitSession::Info &AbstractGitSession::gitinfo()
{
	return m->info;
}

const AbstractGitSession::Info &AbstractGitSession::gitinfo() const
{
	return m->info;
}

AbstractGitSession::GitCache &AbstractGitSession::cache()
{
	return *m->cache;
}

void AbstractGitSession::clearResult()
{
	m->var = {};
}

QString AbstractGitSession::workingDir() const
{
	QString dir = gitinfo().working_repo_dir;
	if (!gitinfo().submodule_path.isEmpty()) {
		dir = dir / gitinfo().submodule_path;
	}
	return dir;
}

void AbstractGitSession::set_command_cache(const GitCommandCache &cc)
{
	m->cache->command_cache = cc;
}
