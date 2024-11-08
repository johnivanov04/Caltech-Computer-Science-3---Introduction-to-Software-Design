#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "router.h"

typedef struct route {
    char *path;
    route_handler_t handler;
} route_t;

struct router {
    route_handler_t fallback;
    size_t max_routes;
    size_t num_routes;
    route_t routes[];
};

router_t *router_init(size_t max_routes, route_handler_t fallback) {
    size_t size = sizeof(router_t) + max_routes * sizeof(route_t);
    router_t *router = (router_t *)malloc(size);
   
    router->fallback = fallback;
    router->max_routes = max_routes;
    router->num_routes = 0;

    return router;
}

void router_register(router_t *router, const char *path, route_handler_t handler) {
    for (size_t i = 0; i < router->num_routes; i++) {
        if (strcmp(router->routes[i].path, path) == 0) {
            router->routes[i].handler = handler;
            free(router->routes[i].path);
            router->routes[i].path = strdup(path);
            return;
        }
    }

    if (router->num_routes < router->max_routes) {
        size_t index = router->num_routes++;
        router->routes[index].path = strdup(path);
        router->routes[index].handler = handler;
    }
}

bytes_t *router_dispatch(router_t *router, request_t *request) {
    if (router == NULL || request == NULL) {
        return NULL;
    }

    bytes_t *response = NULL;
    for (size_t i = 0; i < router->num_routes; i++) {
        if (strcmp(router->routes[i].path, request->path) == 0) {
            response = router->routes[i].handler(request);
            break;
        }
    }

    if (response == NULL) {
        response = router->fallback(request);
    }

    request_free(request);
    return response;
}

void router_free(router_t *router) {
    if (!router) {
        return;
    }

    for (size_t i = 0; i < router->max_routes; i++) {
        free(router->routes[i].path);
    }
    free(router);
}
