#include "ApplicationGlobal.h"
#include "MainWindow.h"
#include <memory>
#include <QFileIconProvider>
#include <QBuffer>
#include "IncrementalSearch.h"
#include "MemoryReader.h"
#include "SimpleQtIO.h"
#include "gzip.h"

struct ApplicationGlobal::Private {
	IncrementalSearch incremental_search;
};

ApplicationGlobal::ApplicationGlobal()
	: m(new Private)
{
}

ApplicationGlobal::~ApplicationGlobal()
{
	delete m;
}

GitContext ApplicationGlobal::gcx()
{
	GitContext gcx;
	gcx.git_command = appsettings.git_command;
	gcx.ssh_command = appsettings.ssh_command;
	return gcx;
}

void ApplicationGlobal::init(QApplication *a)
{
	(void)a;

	bool ok = filetype.open();
	Q_ASSERT(ok);

	graphics = std::make_unique<Graphics>();
	{ // load graphic resources
		QFileIconProvider icons;
		graphics->folder_icon = icons.icon(QFileIconProvider::Folder);
		graphics->repository_icon = QIcon(":/image/repository.png");
		graphics->signature_good_icon = QIcon(":/image/signature-good.png");
		graphics->signature_bad_icon = QIcon(":/image/signature-bad.png");
		graphics->signature_dubious_icon = QIcon(":/image/signature-dubious.png");
		graphics->transparent_pixmap = QPixmap(":/image/transparent.png");
		graphics->small_digits.load(":/image/digits.png");
	}

	m->incremental_search.init();
}

void ApplicationGlobal::writeLog(const std::string_view &str)
{
	mainwindow->emitWriteLog(str);
}

void ApplicationGlobal::writeLog(const QString &str)
{
	mainwindow->emitWriteLog(str);
}

IncrementalSearch *ApplicationGlobal::incremental_search()
{
	return &m->incremental_search;
}

bool ApplicationGlobal::isMainThread()
{
	return QThread::currentThread() == QCoreApplication::instance()->thread();
}

void GlobalSetOverrideWaitCursor()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
}

void GlobalRestoreOverrideCursor()
{
	QApplication::restoreOverrideCursor();
}

struct _AiCredentials : public GenerativeAI::AbstractVisitor<GenerativeAI::Credential> {
	char const *envname;
	_AiCredentials(char const *envname)
		: envname(envname)
	{
	}

	std::string getenv(char const *name)
	{
		char const *value = std::getenv(name);
		return value ? std::string(value) : std::string();
	}


	GenerativeAI::Credential case_Unknown()
	{
		return {};
	}

	GenerativeAI::Credential case_OpenAI()
	{
		GenerativeAI::Credential cred;
		if (global->appsettings.use_env_api_key_OpenAI) {
			cred.api_key = getenv(envname);
		} else {
			cred.api_key = global->appsettings.api_key_OpenAI.toStdString();
		}
		return cred;
	}

	GenerativeAI::Credential case_Anthropic()
	{
		GenerativeAI::Credential cred;
		if (global->appsettings.use_env_api_key_Anthropic) {
			cred.api_key = getenv(envname);
		} else {
			cred.api_key = global->appsettings.api_key_Anthropic.toStdString();
		}
		return cred;
	}
	GenerativeAI::Credential case_Google()
	{
		GenerativeAI::Credential cred;
		if (global->appsettings.use_env_api_key_Google) {
			cred.api_key = getenv(envname);
		} else {
			cred.api_key = global->appsettings.api_key_Google.toStdString();
		}
		return cred;
	}
	GenerativeAI::Credential case_DeepSeek()
	{
		GenerativeAI::Credential cred;
		if (global->appsettings.use_env_api_key_DeepSeek) {
			cred.api_key = getenv(envname);
		} else {
			cred.api_key = global->appsettings.api_key_DeepSeek.toStdString();
		}
		return cred;
	}
	GenerativeAI::Credential case_OpenRouter()
	{
		GenerativeAI::Credential cred;
		if (global->appsettings.use_env_api_key_OpenRouter) {
			cred.api_key = getenv(envname);
		} else {
			cred.api_key = global->appsettings.api_key_OpenRouter.toStdString();
		}
		return cred;
	}

	GenerativeAI::Credential case_Ollama()
	{
		GenerativeAI::Credential cred;
		cred.api_key = "aonymous";
		return cred;
	}

	GenerativeAI::Credential case_LMStudio()
	{
		GenerativeAI::Credential cred;
		cred.api_key = "aonymous";
		return cred;
	}
};

GenerativeAI::Credential ApplicationGlobal::get_ai_credential(GenerativeAI::AI provider)
{
	GenerativeAI::ProviderInfo const *info = provider_info(provider);
	char const *envname = info->env_name.c_str();
	return _AiCredentials(envname).visit(provider);
}

std::string ApplicationGlobal::determineFileType(QByteArray const &in)
{
	if (in.isEmpty()) return {};

	if (in.size() > 10) {
		if (memcmp(in.data(), "\x1f\x8b\x08", 3) == 0) { // gzip
			QBuffer buf;
			MemoryReader mem(in.data(), in.size());

			mem.open(MemoryReader::ReadOnly);
			buf.open(QBuffer::WriteOnly);
			gzip z;
			z.set_maximul_size(100000);
			SimpleQtReader reader(&mem);
			SimpleQtWriter writer(&buf);
			z.decompress(&reader, &writer);

			QByteArray uz = buf.buffer();
			if (!uz.isEmpty()) {
				return filetype.file(uz.data(), uz.size()).mimetype;
			}
		}
	}

	std::string mime = filetype.file(in.data(), in.size()).mimetype;
	// qDebug() << QString::fromStdString(mime);
	return mime;
}

std::string ApplicationGlobal::determineFileType(std::string const &path)
{
	QFile file(QString::fromStdString(path));
	if (file.open(QFile::ReadOnly)) {
		QByteArray ba = file.readAll();
		return determineFileType(ba);
	}
	return {};
}

