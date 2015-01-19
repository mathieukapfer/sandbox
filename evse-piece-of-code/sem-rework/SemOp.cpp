void Lock()
{
    int res;

    struct sembuf op;

    op.sem_num = _semNumber;
    op.sem_op = -1;
    op.sem_flg = 0;

    while(1)
      {
            res = semop(/* Id    */ _semTable,
                        /* Sops  */ &op,
                        /* Nsops */ 1);

            if (res == -1)
            {
                if (errno != EINTR)
                {
                    LOG_FATAL(("Lock  failed for '%s', %s (errno = %d)\n", _name, strerror(errno), errno));

                    if (errno == 13)
                    {
                        LOG_INFO(("Try in superuser mode !!! \n"));
                        exit(0);
                    }

                    break;
                }
            }
            else
            {
                break;
            }
        }

    return res;
}

void Unlock()
{
  struct sembuf op;

  op.sem_num = _semNumber;
  op.sem_op = 1;
  op.sem_flg = 0;

  res = semop(/* Id    */ _semTable,
              /* Sops  */ &op,
              /* Nsops */ 1);
    
}
