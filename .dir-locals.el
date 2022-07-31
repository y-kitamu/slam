((c++-mode
  . (
     (lsp-docker+-server-id . ccls)
     (lsp-docker+-docker-server-id . ccls-docker)
     (lsp-docker+-server-command . "ccls")
     (lsp-docker+-image-id . "arumatik/rosbase")
     (lsp-docker+-container-name . "docker_ros_1")
     (lsp-docker+-server-cmd-fn . lsp-docker+-exec-in-container)
     (lsp-docker+-path-mappings . (("/home/kitamura/work/" . "/home/kitamura/work/")))
     ))
 )