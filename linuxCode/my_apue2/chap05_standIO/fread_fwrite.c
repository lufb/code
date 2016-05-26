#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "../lib/err_msg.h"


#define TMP_FILE "/tmp/lufubo_tmp.data"
struct stu
{
    char            name[32];
    float           score;
};

int
main()
{
    struct stu      student[2], tmp[2];
    FILE            *fp;
    int             rc;

    strcpy(student[0].name, "lufubo1");
    student[0].score = 99;
    strcpy(student[1].name, "lufubo2");
    student[1].score = 100;

    if((fp = fopen(TMP_FILE, "w+")) == NULL)
        pr_msg2exit(1, "open [%s] error[%s]\n", TMP_FILE, strerror(errno));

    if(fwrite(student, sizeof(struct stu), 2, fp) != 2)
    {
        fclose(fp);
        pr_msg2exit(1, "fwrite error[%s]\n", strerror(errno));
    }

    fseek(fp, 0, SEEK_SET);/*重新定位流*/
    if((rc = fread(tmp, sizeof(struct stu), 2, fp)) != 2)
    {
        fclose(fp);
        pr_msg2exit(1, "fread error[%s]\n", strerror(errno));
    }

    printf("stu1[%s:%f]\n", tmp[0].name, tmp[0].score);
    printf("stu2[%s:%f]\n", tmp[1].name, tmp[1].score);

    fclose(fp);

    return 0;
}
