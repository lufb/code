#! /bin/sh

#�÷���sh <directory>
#���ܣ��������directory��������.c,.cpp.c���ļ�ȥ��ע�Ͳ���Ŀ¼�ṹ����ڵ�ǰĿ¼�µĵ�rmd_cmt_file/Ŀ¼��

if [ $# != 1 ]; then
        echo "usage <rm_cmt_directory>"
        exit 1
fi

DIR=$1
PREFIX="rmd_cmt_file/"

if [ ! -d $DIR ]; then
	echo "$DIR is not exist or is a file"
	exit 2
fi


for each in `find $DIR \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \)`
do
	dst=${PREFIX}${each}
	dst_dir=`dirname $dst`
	mkdir -p $dst_dir
	sed 's/a/aA/g;s/__/aB/g;s/b/bA/g;s/#/bB/g' $each |gcc -P -E - | sed 's/bB/#/g;s/bA/b/g;s/aB/__/g;s/aA/a/g' | sed '/^$/d' > $dst
done

echo ' * See result_directory' `pwd`/rmd_cmt_file/

