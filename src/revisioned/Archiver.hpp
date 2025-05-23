#include <vector>

namespace GOTHIC_NAMESPACE
{
	class ArchiveObject
	{
		inline static std::vector<ArchiveObject*> objects;

		bool changeLevelSave{};

	public:

		inline static const zSTRING GLOBAL_ARCHIVE_FILE = "REVISIONED.SAV";
		inline static const zSTRING MAIN_SAVE_FOLDER = "savegame";
		inline static const zSTRING CURRENT_SAVE_FOLDER = "current";
		inline static const zSTRING QUICK_SAVE_FOLDER = "quicksave";

		//todo
		enum class eArchType
		{
			BEGIN,
			END
		};
		eArchType type = eArchType::END;



		virtual void Archive(zCArchiver& t_archiver) = 0;
		virtual void Unarchive(zCArchiver& t_archiver) = 0;


		//called when archiver is not created and we should clean memory
		//virtual void Clear() {};

		ArchiveObject(const bool t_changeLevelSave = 0, eArchType t_archiveMoment = eArchType::END)
			: changeLevelSave(t_changeLevelSave), type(t_archiveMoment)
		{
			objects.push_back(this);
		}

		virtual ~ArchiveObject()
		{
			objects.erase(std::remove(objects.begin(), objects.end(), this));
		}


		static zSTRING GetSlotNameByID(int t_ID)
		{
			if (t_ID > 0)
			{
				return MAIN_SAVE_FOLDER + zSTRING{ t_ID };
			}
			else if (t_ID == 0)
			{
				return QUICK_SAVE_FOLDER;
			}

			return CURRENT_SAVE_FOLDER;
		}

		static zSTRING GetArchivePath(const zSTRING& t_fileName, const int t_slot = -1)
		{
			auto archivePath = zoptions->GetDirString(zTOptionPaths::DIR_SAVEGAMES)
				+ GetSlotNameByID(t_slot)
				+ "\\" + t_fileName;

			return archivePath;
		}

		class BArchiveHelper
		{
			zCArchiver* archiver;

			void Open()
			{
				zSTRING savePath = GetArchivePath(GLOBAL_ARCHIVE_FILE);
				archiver = zarcFactory->CreateArchiverWrite(savePath, zARC_MODE_ASCII, 0, 0);
			}

			void Close()
			{
				if (archiver)
				{
					archiver->Close();
					archiver->Release();
				}
			}

		public:
			BArchiveHelper()
			{
				Open();
			}
			~BArchiveHelper()
			{
				Close();
			}

			zCArchiver* operator()()
			{
				return archiver;
			}
		};


		static void ArchiveAll(const bool t_inChangeLevel)
		{
			//if (SaveLoadGameInfo.changeLevel) return;

			//zSTRING savePath = GetArchivePath(GLOBAL_ARCHIVE_FILE);
			//zCArchiver* archiver = zarcFactory->CreateArchiverWrite(savePath, zARC_MODE_ASCII, 0, 0);
			//if (!archiver){
				//return;
			//}

			BArchiveHelper arch;

			for (const auto& obj : objects)
			{
				if (obj->changeLevelSave == 0
					&& t_inChangeLevel)
				{
					return;
				}
#
				obj->Archive(*arch());
			}

			//archiver->Close();
			//archiver->Release();
		}

		static void UnarchiveAll(const bool t_inChangeLevel)
		{
			//if (SaveLoadGameInfo.changeLevel) return;

			zSTRING savePath = GetArchivePath(GLOBAL_ARCHIVE_FILE);
			zCArchiver* archiver = zarcFactory->CreateArchiverRead(savePath, 0);
			if (!archiver) {
				return;
			}

			for (const auto& obj : objects)
			{
				if (obj->changeLevelSave == 0
					&& t_inChangeLevel)
				{
					return;
				}

				obj->Unarchive(*archiver);
			}

			archiver->Close();
			archiver->Release();
		}

	};
}
