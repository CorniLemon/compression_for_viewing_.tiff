#ifndefTIFREADER_H
#defineTIFREADER_H
#include<stdio.h>
#include<string.h>
#ifndefNULL
#defineNULL0
#endif
#ifndefTRUE
#defineTRUE1
#defineFALSE0
#endif
typedefstruct
{
unsignedshortByte_order;//
unsignedshortVersion;// Проверяем, является ли файл файлом TIF
unsignedintOffsetToFirstFID;// Относительное смещение до начала файла
// unsignedshortwDECount; // Сколько записей в каталоге
}IFH;
typedefstruct
{
unsignedshorttag;// Номер атрибута
unsignedshorttype;// тип данных
unsignedlonglength;// Количество данных
unsignedlongvalueOffset;// Смещение значения переменной, представленной атрибутом тега, от начала файла
}DE;
typedefstruct
{
intwidth;
intheight;
}Size;
typedefstruct
{
int* data;
}DATA;
typedefstruct
{
DE * pde;
intwDECount;
}PDE;
boolreadTIF(char* path, IFH& ifh, PDE& de, Size& size, DATA& Data)
{
	unsignedchar* data;
	int* dat;
	unsignedshortwDECount;// Сколько записей в каталоге
	//ZeroMemory(&ifh,sizeof(IFH));
	//ZeroMemory(&de,sizeof(DE));
	FILE* fp;
	fp = fopen(path, "rb");
	if (fp == NULL)
	{
		cout << "openfileerror" << endl;
		returnfalse;
	}
	if (sizeof(IFH) != fread(&ifh, 1, sizeof(IFH), fp))
	{
		cout << «Не удалось прочитать заголовок файла TIF»;
		returnFALSE;
	}
	if (0x2a != ifh.Version)
	{
		cout << «Файл не в формате TIF, не удалось прочитать файл»;
		returnFALSE;
	}
	if (0x4949 != ifh.Byte_order)
	{
		cout << «Файл TIF имеет неправильный порядок байтов IBMPC.Не удалось прочитать файл»;
		returnFALSE;
	}
	fseek(fp, ifh.OffsetToFirstFID, SEEK_SET);// Находим указатель файла на IFD
	// Сколько записей в каталоге есть для чтения файла
	if (2 != fread(&wDECount, 1, sizeof(unsignedshort), fp))
	{
		cout << «Не удалось получить количество записей каталога файлов TIF»;
		returnFALSE;
	}
	cout << "Файл TIF содержит" << wDECount << "Запись в каталоге" << endl;
	// Создаем массив DE, получаем информацию, в массиве есть элементы wDECount
	de.pde = newDE[wDECount];
	DE* pTemp = de.pde;
	de.wDECount = wDECount;
	memset(de.pde, 0, sizeof(DE) * wDECount);
	if (sizeof(DE) * wDECount != fread(de.pde, 1, sizeof(DE) * wDECount, fp))
	{
		cout << «Не удалось прочитать каталог файлов изображений»;
		delete[]de.pde;
		returnfalse;
	}
	// Сохраняем размер изображения и емкость данных изображения в переменные-члены
	intm_size_x;
	intm_size_y;
	intm_size;
	inti;
	for (i = 0; i < wDECount; i++)
	{
		pTemp = de.pde + i;
		if (256 == pTemp->tag)// Переменная в записи каталога с тегом 256 определяет ширину изображения
		{
			m_size_x = pTemp->valueOffset;
		}
		if (257 == pTemp->tag)// Высота изображения
		{
			m_size_y = pTemp->valueOffset;
		}
		if (273 == pTemp->tag)// Рассчитываем количество байтов, занятых данными изображения
		{
			//m_dwBmSize=pTemp->valueOffset-sizeof(IFH);
			// Или умножьте valueOffset тега = 256 на valueOffset тега = 257
			m_size = m_size_x * m_size_y;
		}
	}
	// Заполняем все пиксельные данные, инвертируем данные изображения и начинаем чтение с последней строки
	intj = 0;
	//inti=0;
	data = (unsignedchar*)malloc(m_size * sizeof(BYTE));
	dat = (int*)malloc(m_size * sizeof(int));
	for (i = m_size_y - 1; i >= 0; i--)
	{
		fseek(fp, sizeof(IFH) + i * m_size_x, SEEK_SET);
		fread((BYTE*)(data + 1) + j * m_size_x, sizeof(BYTE), m_size_x, fp);
		j++;
	}
	cout << "width:" << m_size_x << endl;
	cout << "height:" << m_size_y << endl;
	unsignedchar* p;
	p = data;
	int* ptr;
	ptr = dat;
	for (i = 0; i < m_size; i++, p++, ptr++)
	{
		*ptr = (int)(*p);
		inth = *ptr;
		//cout<<h<<"";
	}
	size.width = m_size_x;
	size.height = m_size_y;
	Data.data = dat;
	returnTRUE;
}
boolsaveTIF(char* path, IFHifh, PDEde, Sizesize, DATAData)
{
	unsignedchar* data;
	// unsignedshortwDECount; // Сколько записей в каталоге
	//ZeroMemory(&ifh,sizeof(IFH));
	//ZeroMemory(&de,sizeof(DE));
	FILE* fp;
	fp = fopen(path, "wb");
	if (fp == NULL)
	{
		cout << "openfileerror" << endl;
		returnfalse;
	}
	if (sizeof(IFH) != fwrite(&ifh, 1, sizeof(IFH), fp))
	{
		cout << «Не удалось записать заголовок файла TIF»;
		returnFALSE;
	}
	fseek(fp, ifh.OffsetToFirstFID, SEEK_SET);// Находим указатель файла на IFD
	// Сколько записей в каталоге есть для чтения файла
	if (2 != fwrite(&de.wDECount, 1, sizeof(unsignedshort), fp))
	{
		cout << «Не удалось получить количество записей каталога файлов TIF»;
		returnFALSE;
	}
	// Создаем массив DE, получаем информацию, в массиве есть элементы wDECount
	if (sizeof(DE) * de.wDECount != fwrite(de.pde, 1, sizeof(DE) * de.wDECount, fp))
	{
		cout << «Не удалось прочитать каталог файлов изображений»;
		returnfalse;
	}
	// Заполняем все пиксельные данные, инвертируем данные изображения и начинаем чтение с последней строки
	intj = 0;
	inti = 0;
	data = (unsignedchar*)malloc(size.width * size.height * sizeof(BYTE));
	int* ptr = Data.data;
	unsignedchar* p;
	p = data;
	for (i = 0; i < size.width * size.height; i++, p++, ptr++)
	{
		*p = (unsignedchar)(*ptr);
		//inth=*ptr;
		//cout<<h<<"";
	}
	for (i = size.height - 1; i >= 0; i--)
	{
		fseek(fp, sizeof(IFH) + i * size.width, SEEK_SET);
		fwrite((BYTE*)(data + 1) + j * size.width, sizeof(BYTE), size.width, fp);
		j++;
	}
	returnTRUE;
}
#endif